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
public:
    static int numrecipes;
    Recipe();
    Recipe(const Recipe& r);
    Recipe& operator=(const Recipe& r);
    ~Recipe();

    void setRecipe();
    void setRecipe(string name, string instructions);
    void setIngredient();
    void setIngredient(string name, int quantity, string quantity_type);
    string getName(){return name;}

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
    void addRecipe(string name, string instructions);
    void HardcodedRecipes();

};

class AlphabeticListing : public RecipeManager {
public:
    void DisplayAlphabetically() {
        bool *visited = new bool[Recipe::numrecipes];
        for (int i = 0; i < Recipe::numrecipes; i++) {
            visited[i] = false;
        }
        for (int i = 0; i < Recipe::numrecipes; i++) {
            int minIndex = -1;


            for (int k = 0; k < Recipe::numrecipes; k++) {
                if (!visited[k]) {
                    if (minIndex == -1 || recipes[k].getName() < recipes[minIndex].getName()) {
                        minIndex = k;
                    }
                }
            }


            cout << recipes[minIndex] << endl;
            visited[minIndex] = true;
        }


        delete[] visited;
    }
};

void HardcodeRecipes() {
}
void Menu(AlphabeticListing& al) {
    int choice;
    do {
        cout << "\n===== Recipe Manager =====\n";
        cout << "  Total recipes : " << Recipe::numrecipes << "\n";
        cout << "  1. Add recipe (user input)\n";
        cout << "  2. Display all recipes \n";
        cout << "  0. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: al.addRecipe();               break;
            case 2: al.DisplayAlphabetically();       break;
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
    Menu(al);
    return 0;
}

Recipe::Recipe() {
    name="";
    instructions="";
    capacity = 10;
    ingredient_count=0;
    ingredients = new Ingredient[capacity];
}

Recipe::Recipe(const Recipe& r) {
    name=r.name;
    instructions=r.instructions;
    ingredient_count=r.ingredient_count;
    capacity=r.capacity;
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
    setIngredient();
}

void Recipe::setRecipe(string name, string instructions) {
    this->name = name;
    this->instructions = instructions;
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

void RecipeManager::addRecipe(string name, string instructions) {
    increaseCapacity();
    recipes[stored_recipes].setRecipe(name,instructions);
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