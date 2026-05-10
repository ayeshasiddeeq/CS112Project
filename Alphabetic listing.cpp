#include <iostream>
using namespace std;

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

class Recipe {
private:
    string name;
    string instructions;
    Ingredient* ingredients;
    int ingredient_count;
    int capacity;
    int cookingTime;          //added cooking time
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
    int getCookingTime() { return cookingTime; }

    int getIngredientCount();

    friend ostream &operator<<(ostream &os, const Recipe &recipe);
};
int Recipe::numrecipes = 0;

class RecipeManager{
protected:
    Recipe *recipes;
    int recipe_capacity;
    int stored_recipes;

public:
    RecipeManager();
    ~RecipeManager();

    void increaseCapacity();
    void addRecipe();
    void addRecipe(string name, string instructions,int time);
    void HardcodedRecipes();
    int getStoredCount() { return stored_recipes; }
    Recipe& getRecipeAt(int i) { return recipes[i]; }

};

class AlphabeticListing : public RecipeManager {
public:
    void DisplayAlphabetically() {
        if (stored_recipes == 0) {
            cout << "No recipes stored.\n";
            return;
        }
        bool *visited = new bool[stored_recipes];
        for (int i = 0; i < stored_recipes; i++) {
            visited[i] = false;
        }
        for (int i = 0; i < stored_recipes; i++) {
            int minIndex = -1;


            for (int k = 0; k < stored_recipes; k++) {
                if (!visited[k]) {
                    if (minIndex == -1 || recipes[k].getName() < recipes[minIndex].getName()) {
                        minIndex = k;
                    }
                }
            }

            cout<<minIndex+1<<" ";
            cout << recipes[minIndex] << endl;
            visited[minIndex] = true;
        }


        delete[] visited;
    }
};
class RecipeRanker : public RecipeManager {
private:
    int* selectedRecipe;   // indices of chosen recipes
    int selectionCount;
    int selectionCapacity;
    // --- Scoring logic ---
    // Lower score = better recipe
    // We reward fewer ingredients and shorter cooking time
    float computeScore(int recipeIndex) {
        int time = recipes[recipeIndex].getCookingTime();
        int ingCount = recipes[recipeIndex].getIngredientCount();

        // Both factors equally weighted — you can adjust the multipliers
        // Lower time and fewer ingredients = lower (better) score
        return (float)(time * 1.0 + ingCount * 5.0);
        //                 ^                  ^
        //          each minute counts    each ingredient
        //          as 1 point            counts as 5 points
    }

public:
    void syncFrom(RecipeManager& source) {
        delete[] recipes;
        recipe_capacity = source.getStoredCount() + 10;  // some buffer
        recipes = new Recipe[recipe_capacity];
        stored_recipes = 0;
        for (int i = 0; i < source.getStoredCount(); i++) {
            recipes[stored_recipes] = source.getRecipeAt(i);
            stored_recipes++;
        }
    }
    RecipeRanker() {
        selectionCapacity = 5;
        selectionCount = 0;
        selectedRecipe = new int[selectionCapacity];
    }

    ~RecipeRanker() {
        delete[] selectedRecipe;
    }
    // User picks 2 or 3 recipes by index number
    void selectRecipesToRank() {
        selectionCount = 0;   // reset previous selection
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
    // Rank selected recipes — selection sort by score (ascending = best first)
    void rankAndDisplay() {
        if (selectionCount < 2) {
            cout << "Select recipes first.\n";
            return;
        }
        // Copy selected indices into a working array so we don't modify the original
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
        // Display ranked results
        cout << "\n======= RANKING (Best to Worst) =======\n";
        for (int i = 0; i < selectionCount; i++) {
            int idx = ranked[i];
            cout << "  Rank #" << (i + 1) << ": " << recipes[idx].getName() << "\n";
        }

        // Highlight the winner
        cout << " Best recommendation: " << recipes[ranked[0]] << " <<<\n";
        cout << "=======================================\n";

        delete[] ranked;
    }
    // Convenience: do both steps in sequence
    void runRanking() {
        selectRecipesToRank();
        rankAndDisplay();
    }
};

void HardcodeRecipes() {
}
void Menu(AlphabeticListing& al, RecipeRanker& rank) {
    int choice;
    do {
        cout << "\n===== Recipe Manager =====\n";
        cout << "  Total recipes : " << Recipe::numrecipes << "\n";
        cout << "  1. Add recipe (user input)\n";
        cout << "  2. Display all recipes \n";
        cout << "  3. Rank recipes\n";
        cout << "  0. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: al.addRecipe();
                rank.syncFrom(al);
                break;
            case 2: al.DisplayAlphabetically();       break;
            case 3: rank.runRanking();        break;
            case 0:
                cout << "Goodbye!\n";
                break;
            default:
                cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

int main() {
    AlphabeticListing al;
    RecipeRanker ranker;
    Menu(al,ranker);
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
    getline(cin,name);
    cout<<"Recipe instructions: ";
    getline(cin,instructions);
    cout << "Cooking time (in minutes): ";
    cin >> cookingTime;
    cin.ignore();
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
    string ingredName;
    cout<<"\nEnter ingredients (type \"done\" to finish)."<<endl;
    while (true) {
            cout<<"Ingredient :";
            getline(cin, ingredName);
            if (ingredName=="done")
                break;
            int quantity;
            string quantity_type;

            cout<<"Quantity:";
            cin >> quantity;
            cin.ignore();

            cout<<"Quantity type (tablespoons / teaspoons / cups /grams etc) :";
            getline(cin,quantity_type);

            setIngredient(ingredName,quantity,quantity_type);
    }
    cout<<endl;
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
        <<"\t"<<recipe.ingredients[i].quantity_type;
    }
    os<<"Instructions: "<<recipe.instructions<<endl;
    os<<"*****"<<endl;
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

void HardcodedRecipes() {
    /*hardcode recipes into this
     *use this format
     *addRecipe(name,instructions)
     *addIngredientToRecipe(index,name, quantity, quantity type)
     *add more ingredients ust like this
     *repeat for as many recipes as required
     */
}




