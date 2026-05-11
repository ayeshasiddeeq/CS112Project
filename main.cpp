#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cctype>
using namespace std;

class RecipeNotFoundException : public exception {
private:
    string message;
public:
    RecipeNotFoundException(const string& name) {
        message = "Recipe not found: \"" + name + "\"";
    }
    const char* what() const noexcept override { return message.c_str(); }
};

class InvalidInputException : public exception {
private:
    string message;
public:
    InvalidInputException(const string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class Displayable {
public:
    virtual void display() = 0;
    virtual ~Displayable() {}
};

class Ingredient{
public:
    string name;
    int quantity;
    string quantity_type;
    Ingredient() {
        name="";
        quantity=0;
        quantity_type="";
    }
    Ingredient(string name) {
        this->name = name;
        quantity=0;
        quantity_type="";
    }
    Ingredient(string name,int quantity, string type) {
        this->name = name;
        this->quantity = quantity;
        this->quantity_type = type;
    }
};
class Saveable {
public:
    virtual void saveToFile(const string& filename) = 0;
    virtual bool loadFromFile(const string& filename) = 0;
    virtual ~Saveable() {}
};

class Recipe {
private:
    string name;
    string instructions;
    Ingredient* ingredients;
    int ingredient_count;
    int capacity;
    int cookingTime;
public:
    static int numrecipes;
    Recipe();
    Recipe(const Recipe& r);
    Recipe& operator=(const Recipe& r);
    ~Recipe();

    void setRecipe();
    void setRecipe(string name, string instructions,int time);
    void setIngredient();
    void setIngredient(string name, int quantity, string quantity_type);
    string getName(){return name;}
    string getInstructions() { return instructions; }
    int getCookingTime() { return cookingTime; }
    string getIngredientName(int i) const;
    int getIngredientCount();

    friend ostream &operator<<(ostream &os, const Recipe &recipe);
    friend class RecipeManager;
};
int Recipe::numrecipes = 0;

class RecipeManager:public Displayable,public Saveable{
protected:
    Recipe *recipes;
    int recipe_capacity;
    int stored_recipes;

public:
    RecipeManager();
    ~RecipeManager();

    void display() override{
        for (int i = 0; i < stored_recipes; i++)
            cout << (i + 1) << ". " << recipes[i];
    }
    void increaseCapacity();
    void addRecipe();
    void addRecipe(string name, string instructions,int time);
    void HardcodeRecipes();
    int getStoredCount() { return stored_recipes; }
    void searchByIngredients();
    virtual void saveToFile(const string& filename) override;
    virtual bool loadFromFile(const string& filename) override;

    void searchByName(const string& query);

    Recipe& getRecipeAt(int i) { return recipes[i]; }
  };
class AlphabeticListing : public RecipeManager {
public:
    void display();

};

class RecipeRanker : public RecipeManager {
private:
    int* selectedRecipe;
    int selectionCount;
    int selectionCapacity;

    float computeScore(int recipeIndex) {
        int time = recipes[recipeIndex].getCookingTime();
        int ingCount = recipes[recipeIndex].getIngredientCount();
        return (float)(time * 1.0 + ingCount * 5.0);
    }

public:
    RecipeRanker() {
        selectionCapacity = 5;
        selectionCount = 0;
        selectedRecipe = new int[selectionCapacity];
    }

    ~RecipeRanker() {
        delete[] selectedRecipe;
    }
    void syncFrom(RecipeManager& source);
    void selectRecipesToRank();
    void rankAndDisplay();
    void runRanking() {
        selectRecipesToRank();
        rankAndDisplay();
    }
};

void Menu(AlphabeticListing& al, RecipeRanker& rank);

int main() {
    AlphabeticListing al;
    RecipeRanker ranker;
    if (!al.loadFromFile("recipes.txt")) {
        cout << "First run - loading default recipes and saving to file...\n";
    al.HardcodeRecipes();
    al.saveToFile("recipes.txt");
    }
    ranker.syncFrom(al);
    Menu(al, ranker);
    return 0;
    }

Recipe::Recipe() {
    name="";
    instructions="";
    capacity = 10;
    ingredient_count=0;
    cookingTime=0;
    ingredients = new Ingredient[capacity];
}

Recipe::Recipe(const Recipe& r) {
    name=r.name;
    instructions=r.instructions;
    ingredient_count=r.ingredient_count;
    capacity=r.capacity;
    cookingTime = r.cookingTime;
    ingredients= new Ingredient[capacity];
    for (int i=0 ; i<ingredient_count;i++) {
        ingredients[i]=r.ingredients[i];
    }
}

Recipe &Recipe::operator=(const Recipe &r) {
    if (this ==&r)
        return *this;
    delete []ingredients;
    name=r.name;
    instructions=r.instructions;
    ingredient_count=r.ingredient_count;
    capacity=r.capacity;
    cookingTime = r.cookingTime;
    ingredients = new Ingredient[capacity];
    for (int i = 0; i<ingredient_count;i++) {
        ingredients[i]=r.ingredients[i];
    }
    return *this;
}


Recipe::~Recipe() {
    delete []ingredients;
}
void Recipe::setRecipe() {
    cout<<"Recipe name: ";
    cin.ignore();
    getline(cin,name);
    cout<<"Recipe instructions: ";
    getline(cin,instructions);
    while (true) {
        cout << "Cooking time (minutes): ";
        try {
            if (!(cin >> cookingTime) || cookingTime <= 0) {
                cin.clear(); cin.ignore(10000, '\n');
                throw InvalidInputException(
                    "Cooking time must be a positive whole number.");
            }
            cin.ignore();
            break;
        }
        catch (const InvalidInputException& e) {
            cout << "  Error: " << e.what() << "\n";
        }
    }
    setIngredient();
}

void Recipe::setRecipe(string name, string instructions, int time) {
    this->name = name;
    this->instructions = instructions;
    this->cookingTime = time;
}


void Recipe::setIngredient(string name, int quantity, string quantity_type) {
    if (ingredient_count==capacity) {
        capacity *= 2;
        Ingredient *bigger = new Ingredient[capacity];
        for (int i = 0; i<ingredient_count;i++) {
            bigger[i]=ingredients[i];
        }
        delete []ingredients;
        ingredients = bigger;
    }
    ingredients[ingredient_count++]=Ingredient(name,quantity,quantity_type);
}

void Recipe::setIngredient(){
    cout << "\nEnter ingredients (type \"done\" to finish).\n";
    while (true) {
        cout << "  Ingredient name: ";
        string ingredName;
        getline(cin, ingredName);
        if (ingredName == "done") break;
        int qty;
        while (true) {
            cout << "  Quantity: ";
            try {
                if (!(cin >> qty) || qty <= 0) {
                    cin.clear(); cin.ignore(10000, '\n');
                    // REQUIREMENT h.i — throw
                    throw InvalidInputException(
                        "Quantity must be a positive whole number.");
                }
                cin.ignore();
                break;
            }
            catch (const InvalidInputException& e) {
                cout << "  Error: " << e.what() << "\n";
            }
        }
        string qtype;
        cout << "  Unit (grams/cups/tbsp/tsp/pieces etc.): ";
        getline(cin, qtype);
        setIngredient(ingredName, qty, qtype);
    }
    cout << "\n";
}


string Recipe::getIngredientName(int i) const {
    if (i >= 0 && i < ingredient_count) {
        return ingredients[i].name;
    }
    else {
        return "";
    }
}

int Recipe::getIngredientCount() {
    return ingredient_count;
}

ostream &operator<<(ostream &os, const Recipe &recipe) {
    os<<"Recipe name: "<<recipe.name<<endl;
    os<<"Ingredients:\n";
    for (int i = 0;i<recipe.ingredient_count;i++) {
        os<<"\t"<<recipe.ingredients[i].name
        <<" - "<<recipe.ingredients[i].quantity
        <<"\t"<<recipe.ingredients[i].quantity_type<<endl;
    }
    os<<endl<<"Instructions: "<<recipe.instructions<<endl;
    os<<endl;
    return os;
}

RecipeManager::RecipeManager() {
    recipe_capacity = 10;
    recipes = new Recipe[recipe_capacity];
    stored_recipes=0;
}

RecipeManager::~RecipeManager() {
    delete []recipes;
}

void RecipeManager::increaseCapacity() {
    if (stored_recipes < recipe_capacity) {
        return;
    }
    int new_capacity = recipe_capacity*2;
    Recipe *bigger = new Recipe[new_capacity];
    for (int i = 0; i<stored_recipes;i++) {
        bigger[i] = recipes[i];
    }
    delete[] recipes;
    recipes = bigger;
    recipe_capacity = new_capacity;
}
void RecipeManager::addRecipe() {
    increaseCapacity();
    recipes[stored_recipes].setRecipe();
    stored_recipes++;
    Recipe::numrecipes++;
}

void RecipeManager::addRecipe(string name, string instructions, int time) {
    increaseCapacity();
    recipes[stored_recipes].setRecipe(name,instructions,time);
    stored_recipes++;
    Recipe::numrecipes++;
}

void RecipeManager::searchByIngredients() {
    if (stored_recipes == 0) { cout << "No recipes stored.\n"; return; }
    const int  MAX_ING   = 50;
    string*    userIngs  = new string[MAX_ING];
    int        userIngCnt = 0;

    cout << "\nEnter ingredients you have (type \"done\" to finish):\n";
    while (userIngCnt < MAX_ING) {
        cout << "  Ingredient: ";
        string ing;
        getline(cin, ing);
        if (ing == "done") break;
        for (int k = 0; k < (int)ing.size(); k++) ing[k] = tolower(ing[k]);
        userIngs[userIngCnt++] = ing;
    }

    if (userIngCnt == 0) {
        cout << "No ingredients entered.\n";
        delete[] userIngs;
        return;
    }

    int* matchCount = new int[stored_recipes];
    for (int i = 0; i < stored_recipes; i++) {
        matchCount[i] = 0;
        for (int j = 0; j < recipes[i].getIngredientCount(); j++) {
            string recIng = recipes[i].getIngredientName(j);
            for (int k = 0; k < (int)recIng.size(); k++)
                recIng[k] = tolower(recIng[k]);

            for (int u = 0; u < userIngCnt; u++) {
                if (recIng.find(userIngs[u]) != string::npos ||
                    userIngs[u].find(recIng)  != string::npos) {
                    matchCount[i]++;
                    break;
                    }
            }
        }
    }

    int* order = new int[stored_recipes];
    for (int i = 0; i < stored_recipes; i++) order[i] = i;
    for (int i = 0; i < stored_recipes - 1; i++) {
        int best = i;
        for (int j = i + 1; j < stored_recipes; j++)
            if (matchCount[order[j]] > matchCount[order[best]]) best = j;
        int tmp = order[i]; order[i] = order[best]; order[best] = tmp;
    }

    cout << "\n====== Recipes Matching Your Ingredients ======\n";
    bool anyMatch = false;
    for (int i = 0; i < stored_recipes; i++) {
        int idx = order[i];
        if (matchCount[idx] > 0) {
            cout << "  [ " << matchCount[idx] << " ingredient match(es) ]\n";
            cout << recipes[idx];
            anyMatch = true;
        }
    }
    if (!anyMatch) cout << "  No recipes matched your ingredients.\n";
    cout << "================================================\n";

    delete[] userIngs;
    delete[] matchCount;
    delete[] order;
}

void RecipeManager::searchByName(const string& query) {
    string lowerQuery = query;
    for (int k = 0; k < (int)lowerQuery.size(); k++)
        lowerQuery[k] = tolower(lowerQuery[k]);
    bool found = false;
    cout << "\n====== Search Results ======\n";
    for (int i = 0; i < stored_recipes; i++) {
        string lowerName = recipes[i].getName();
        for (int k = 0; k < (int)lowerName.size(); k++)
            lowerName[k] = tolower(lowerName[k]);

        if (lowerName.find(lowerQuery) != string::npos) {
            cout << recipes[i];
            found = true;
        }
    }
    if (!found) throw RecipeNotFoundException(query);
    cout << "============================\n";
}

void AlphabeticListing::display() {
    if (stored_recipes == 0) { cout << "No recipes stored.\n"; return; }

    // REQUIREMENT f.iv — Selection sort by recipe name
    bool* visited = new bool[stored_recipes];
    for (int i = 0; i < stored_recipes; i++) visited[i] = false;

    cout << "\n========== All Recipes (A-Z) ==========\n";
    for (int pass = 0; pass < stored_recipes; pass++) {
        int minIdx = -1;
        for (int k = 0; k < stored_recipes; k++) {
            if (!visited[k]) {
                if (minIdx == -1 ||
                    recipes[k].getName() < recipes[minIdx].getName())
                    minIdx = k;
            }
        }
        cout << (pass + 1) << ". " << recipes[minIdx];
        visited[minIdx] = true;
    }
    cout << "========================================\n";
    delete[] visited;
}

void RecipeRanker::syncFrom(RecipeManager& source) {
    delete[] recipes;
    recipe_capacity = source.getStoredCount() + 10;
    recipes         = new Recipe[recipe_capacity];
    stored_recipes  = 0;
    for (int i = 0; i < source.getStoredCount(); i++)
        recipes[stored_recipes++] = source.getRecipeAt(i);
}
void RecipeRanker::selectRecipesToRank() {
    selectionCount = 0;
    int howMany;
    cout << "\nHow many recipes to compare? (2 or 3): ";
    cin >> howMany;
    cin.ignore();

    if (howMany < 2 || howMany > 3) {
        cout << "Please enter 2 or 3.\n";
        return;
    }
    if (howMany > stored_recipes) {
        cout << "Not enough recipes stored.\n";
        return;
    }

    for (int i = 0; i < howMany; i++) {
        int idx;
        cout << "Enter index of recipe " << (i + 1) << ": ";
        cin >> idx;
        cin.ignore();
        // Validate index
        if (idx < 0 || idx >= stored_recipes) {
            cout << "Invalid index. Try again.\n";
            i--;   // repeat this iteration
            continue;
        }
        // Check for duplicates
        bool duplicate = false;
        for (int j = 0; j < selectionCount; j++) {
            if (selectedRecipe[j] == idx) {
                cout << "Already selected. Pick a different one.\n";
                duplicate = true;
                break;
            }
        }
        if (duplicate) { i--; continue; }
        selectedRecipe[selectionCount++] = idx;
    }
}

void RecipeRanker::rankAndDisplay() {
    if (selectionCount < 2) {
        cout << "Select recipes first.\n";
        return;
    }
    int* ranked = new int[selectionCount];
    for (int i = 0; i < selectionCount; i++)
        ranked[i] = selectedRecipe[i];

    // Selection sort — find the minimum score each pass
    for (int i = 0; i < selectionCount - 1; i++) {
        int bestPos = i;
        for (int j = i + 1; j < selectionCount; j++) {
            if (computeScore(ranked[j]) < computeScore(ranked[bestPos]))
                bestPos = j;
        }
        // Swap
        int temp = ranked[i];
        ranked[i] = ranked[bestPos];
        ranked[bestPos] = temp;
    }

    cout << "\n======= RANKING (Best to Worst) =======\n";
    for (int i = 0; i < selectionCount; i++) {
        int idx = ranked[i];
        cout << "  Rank #" << (i + 1) << ": " << recipes[idx].getName() << "\n";
    }

    cout << " Best recommendation: " << recipes[ranked[0]] << " <<<\n";
    cout << "=======================================\n";

    delete[] ranked;
}

void Menu(AlphabeticListing& al, RecipeRanker& rank) {
    int choice;
    do {
        cout << "\n===== Recipe Manager =====\n";
        cout << "  Total recipes : " << Recipe::numrecipes << "\n";
        cout << "  1. Add recipe\n";
        cout << "  2. Display all recipes (A-Z)\n";
        cout << "  3. Rank recipes\n";
        cout << "  4. Search by ingredients\n";
        cout << "  5. Search by name\n";
        cout << "  0. Exit\n";
        cout << "Choice: ";
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(10000, '\n');
            cout << "  Invalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore();

        switch (choice) {
            case 1:
                al.addRecipe();
                rank.syncFrom(al);
                al.saveToFile("recipes.txt");
                break;

            case 2:
                al.display();
                break;

            case 3:
                rank.runRanking();
                break;

            case 4:
                al.searchByIngredients();
                break;
            case 5: {
                cout << "  Enter name (or part of name) to search: ";
                cin.ignore();
                string query;
                getline(cin, query);
                al.searchByName(query);
                break;
            }
            case 0:
                cout << "Goodbye!\n";
                break;
            default:
                cout << "  Invalid choice.\n";
        }
    } while (choice != 0);
}

void RecipeManager::saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Warning: Could not open \"" << filename << "\" for saving.\n";
        return;
    }
    for (int i = 0; i < stored_recipes; i++) {
        file << "===RECIPE===\n";
        file << recipes[i].getName()            << "\n";
        file << recipes[i].getCookingTime()     << "\n";
        file << recipes[i].getIngredientCount() << "\n";
        // Direct member access permitted via "friend class RecipeManager"
        for (int j = 0; j < recipes[i].getIngredientCount(); j++) {
            file << recipes[i].ingredients[j].name     << "|"
                 << recipes[i].ingredients[j].quantity << "|"
                 << recipes[i].ingredients[j].quantity_type << "\n";
        }
        file << recipes[i].getInstructions() << "\n";
        file << "===END===\n";
    }
    file.close();
}

bool RecipeManager::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;   // first run

    string line;
    while (getline(file, line)) {
        if (line != "===RECIPE===") continue;

        string recipeName, instructions;
        int    cookTime = 0, ingCount = 0;

        getline(file, recipeName);

        file >> cookTime;  file.ignore();
        file >> ingCount;  file.ignore();

        addRecipe(recipeName, "", cookTime);
        int idx = stored_recipes - 1;

        for (int j = 0; j < ingCount; j++) {
            if (!getline(file, line)) break;
            // Parse "name|quantity|unit"
            size_t p1 = line.find('|');
            if (p1 == string::npos) continue;
            size_t p2 = line.find('|', p1 + 1);
            if (p2 == string::npos) continue;

            string iName = line.substr(0, p1);
            int    iQty  = stoi(line.substr(p1 + 1, p2 - p1 - 1));
            string iType = line.substr(p2 + 1);
            recipes[idx].setIngredient(iName, iQty, iType);
        }

        // Read instructions line (may be empty string for programmatic recipes)
        if (getline(file, instructions))
            recipes[idx].instructions = instructions; // friend class access

        getline(file, line);  // consume "===END==="
    }
    file.close();
    return true;
}
void RecipeManager::HardcodeRecipes() {
    addRecipe("Spaghetti Aglio e Olio","Boil spaghetti in salted water until al dente. In a pan, heat olive oil, sauté sliced garlic until golden. Add chili flakes. Toss cooked spaghetti in the oil mixture. Add salt and parsley. Serve hot.",25);
    recipes[stored_recipes-1].setIngredient("Spaghetti", 200, "grams");
    recipes[stored_recipes-1].setIngredient("Garlic",4,"cloves");
    recipes[stored_recipes-1].setIngredient("Olive Oil",3,"tbsp");
    recipes[stored_recipes-1].setIngredient("Red Chilli Flakes",1,"tsp");
    recipes[stored_recipes-1].setIngredient("Salt",1,"tsp");
    recipes[stored_recipes-1].setIngredient("Parsley",2,"tbsp");

    addRecipe("Chicken Karahi","Heat oil, add chicken and cook until color changes. Add ginger-garlic paste. Add tomatoes and spices. Cook until oil separates. Garnish with green chilies and coriander.",50);
    recipes[stored_recipes-1].setIngredient("Chicken", 500, "grams");
    recipes[stored_recipes-1].setIngredient("Tomatoes",3,"pieces");
    recipes[stored_recipes-1].setIngredient("Garlic",1,"tbsp");
    recipes[stored_recipes-1].setIngredient("Ginger",1,"tbsp");
    recipes[stored_recipes-1].setIngredient("Oil",4,"tbsp");
    recipes[stored_recipes-1].setIngredient("Red Chilli Powder",1,"tsp");
    recipes[stored_recipes-1].setIngredient("Salt",1,"tsp");
    recipes[stored_recipes-1].setIngredient("Green Chillies",2,"tbsp");

    addRecipe("Teriyaki Chicken","Mix soy sauce, honey, garlic, and ginger. Marinate chicken for 20 minutes. Cook chicken in pan until golden. Pour sauce and simmer until thick.",30);
    recipes[stored_recipes-1].setIngredient("Chicken Breast", 300, "grams");
    recipes[stored_recipes-1].setIngredient("Garlic",2,"cloves");
    recipes[stored_recipes-1].setIngredient("Oil",2,"tbsp");
    recipes[stored_recipes-1].setIngredient("Soy Sauce",4,"tbsp");
    recipes[stored_recipes-1].setIngredient("Ginger",1,"tsp");
    recipes[stored_recipes-1].setIngredient("Honey",2,"tbsp");

    addRecipe("Chicken Taco","Cook chicken with seasoning. Warm tortillas. Fill with chicken, veggies, and cheese. Serve fresh.",30);
    recipes[stored_recipes-1].setIngredient("Chicken", 250, "grams");
    recipes[stored_recipes-1].setIngredient("Onion",1,"piece");
    recipes[stored_recipes-1].setIngredient("Tomato",1,"piece");
    recipes[stored_recipes-1].setIngredient("Lettuce",1,"cup");
    recipes[stored_recipes-1].setIngredient("Oil",3,"tbsp");
    recipes[stored_recipes-1].setIngredient("Cheese",1,"cup");
    recipes[stored_recipes-1].setIngredient("Taco Seasoning",1,"tbsp");
    recipes[stored_recipes-1].setIngredient("Tortilla",4,"pieces");

    addRecipe("Egg Fried Rice","Heat oil, scramble eggs. Add vegetables, then rice. Add soy sauce and salt. Stir fry on high heat.",25);
    recipes[stored_recipes-1].setIngredient("Boiled rice", 2, "cups");
    recipes[stored_recipes-1].setIngredient("Eggs",2,"pieces");
    recipes[stored_recipes-1].setIngredient("Oil",2,"tbsp");
    recipes[stored_recipes-1].setIngredient("Carrot",1,"cup");
    recipes[stored_recipes-1].setIngredient("Salt",1,"tsp");
    recipes[stored_recipes-1].setIngredient("Soy sauce",2,"tbsp");

    addRecipe("Butter Chicken","Marinate chicken in yogurt and spices. Cook chicken. Prepare tomato gravy with butter and spices. Add chicken and cream. Simmer.",60);
    recipes[stored_recipes-1].setIngredient("Chicken", 500, "grams");
    recipes[stored_recipes-1].setIngredient("Yoghurt",1,"cup");
    recipes[stored_recipes-1].setIngredient("Tomato puree",1,"cup");
    recipes[stored_recipes-1].setIngredient("Butter", 3, "tbsp");
    recipes[stored_recipes-1].setIngredient("Cream",1,"cup");
    recipes[stored_recipes-1].setIngredient("Red Chilli Powder",1,"tsp");
    recipes[stored_recipes-1].setIngredient("Salt",1,"tsp");
    recipes[stored_recipes-1].setIngredient("Garam masala",1,"tsp");

    addRecipe("Greek Salad","Chop vegetables. Mix everything in bowl. Add olive oil, lemon juice, and salt. Toss gently.",10);
    recipes[stored_recipes-1].setIngredient("Feta Cheese", 100, "grams");
    recipes[stored_recipes-1].setIngredient("Cucumber",1,"piece");
    recipes[stored_recipes-1].setIngredient("Olive Oil",2,"tbsp");
    recipes[stored_recipes-1].setIngredient("Tomato",2,"pieces");
    recipes[stored_recipes-1].setIngredient("Salt",1,"tsp");
    recipes[stored_recipes-1].setIngredient("Lemon juice",1,"tbsp");
    recipes[stored_recipes-1].setIngredient("Olives",1,"cup");

    addRecipe("Classic Beef Burger","Grill beef patty. Toast buns. Assemble burger with lettuce, tomato, cheese, and sauces.",35);
    recipes[stored_recipes-1].setIngredient("Beef patty", 200, "grams");
    recipes[stored_recipes-1].setIngredient("Burger buns",2,"pieces");
    recipes[stored_recipes-1].setIngredient("Lettuce",2,"leaves");
    recipes[stored_recipes-1].setIngredient("Tomato",2,"slices");
    recipes[stored_recipes-1].setIngredient("Ketchup",1,"tbsp");
    recipes[stored_recipes-1].setIngredient("Mayonnaise",1,"tbsp");
    recipes[stored_recipes-1].setIngredient("Cheese slice",1,"piece");

    addRecipe("Hummus","Blend chickpeas, tahini, garlic, lemon juice, and salt. Add olive oil. Blend until smooth.",10);
    recipes[stored_recipes-1].setIngredient("Chickpeas", 1, "cup");
    recipes[stored_recipes-1].setIngredient("Garlic",2,"cloves");
    recipes[stored_recipes-1].setIngredient("Olive Oil",2,"tbsp");
    recipes[stored_recipes-1].setIngredient("Tahini",2,"tbsp");
    recipes[stored_recipes-1].setIngredient("Salt",1,"tsp");
    recipes[stored_recipes-1].setIngredient("Lemon uice",2,"tbsp");

    addRecipe("Crepes","Mix all ingredients into smooth batter. Heat pan and pour thin layer. Cook both sides until golden. Serve with toppings.",25);
    recipes[stored_recipes-1].setIngredient("Flour", 1, "cup");
    recipes[stored_recipes-1].setIngredient("Milk",1,"cup");
    recipes[stored_recipes-1].setIngredient("Eggs",2,"pieces");
    recipes[stored_recipes-1].setIngredient("Sugar",1,"tbsp");
    recipes[stored_recipes-1].setIngredient("Salt",1,"tsp");
    recipes[stored_recipes-1].setIngredient("Butter",2,"tbsp");
}