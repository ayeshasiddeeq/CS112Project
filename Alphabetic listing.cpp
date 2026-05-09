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
    void setRecipe();
    void setRecipe(string name, string instructions);
    void setIngredient();
    void setIngredient(string name, int quantity, string quantity_type);
    int getIngredientCount();
    string getName() { return name; }
    friend ostream &operator<<(ostream &os, const Recipe &recipe);
    ~Recipe() {
        delete []ingredients;
        numrecipes--;
    }
};
int Recipe::numrecipes = 0;


class RecipeManager{
protected:
    Recipe *recipes;
    int recipe_capacity;

public:
    RecipeManager() {
        recipe_capacity = 10;
        recipes = new Recipe[recipe_capacity];
        int stored_recipes;
    }
    void DisplayAllRecipes();
    void addRecipe();
    void addRecipe(string name, string instructions);
    ~RecipeManager() {
        delete []recipes;
    }
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

int main(){
    RecipeManager manager;
    manager.addRecipe();
}

Recipe::Recipe() {
    name="";
    instructions="";
    capacity = 10;
    ingredient_count=0;
    ingredients = new Ingredient[capacity];
}
void Recipe::setRecipe() {
    cout<<"Recipe name: ";
    getline(cin,name);
    setIngredient();
    cout<<"Recipe instructions: ";
    getline(cin,instructions);
}

void Recipe::setRecipe(string name, string instructions) {
    this->name = name;
    this->instructions = instructions;
}


void Recipe::setIngredient(string name, int quantity, string quantity_type) {
    this->ingredients->name = name;
    this->ingredients->quantity = quantity;
    this->ingredients->quantity_type = quantity_type;
}

void Recipe::setIngredient(){
    for (int i = 0; i < capacity; i++) {
        cout<<"Ingredient "<<i<<" :";
        getline(cin,ingredients[i].name);
        cin.ignore();
        cout<<"Quantity:";
        cin >> ingredients[i].quantity;
        cin.ignore();
        cout<<"Quantity type (tablespoons / teaspoons / cups etc) :";
        getline(cin,ingredients[i].quantity_type);
        cin.ignore();
    }
}

int Recipe::getIngredientCount() {
    return ingredient_count;
}

ostream &operator<<(ostream &os, const Recipe &recipe) {
    os<<"Recipe name: "<<recipe.name<<endl;
    for (int i = 0; i < recipe.ingredient_count; i++) {
        os<<recipe.ingredients[i].name<<"\t";
        os<<recipe.ingredients[i].quantity<<"\t";
        os<<recipe.ingredients[i].quantity_type<<endl;
    }
    os<<recipe.instructions<<endl<<"*****"<<endl;
    return os;
}

void RecipeManager::DisplayAllRecipes() {
    if (recipes==NULL) {
        cout<<"There are no recipes stored. "<<endl;
        return;
    }
    for (int i = 0; i < recipes->getIngredientCount(); i++) {
        cout<<recipes[i];
    }
}
void RecipeManager::addRecipe() {
    if (recipes->numrecipes ==recipe_capacity) {
        int new_capacity = recipe_capacity * 2;
        Recipe *new_recipies = new Recipe[new_capacity];

        for (int i = 0; i < recipes->numrecipes; i++) {
            new_recipies[i] = recipes[i];
        }
        delete []recipes;
        recipes = new_recipies;
        recipe_capacity = new_capacity;
    }
    Recipe temp;
    temp.setRecipe();
    recipes[recipes->numrecipes++] = temp;
}
void RecipeManager::addRecipe(string name, string instructions) {
    if (recipes->numrecipes ==recipe_capacity) {
        int new_capacity = recipe_capacity * 2;
        Recipe *new_recipies = new Recipe[new_capacity];

        for (int i = 0; i < recipes->numrecipes; i++) {
            new_recipies[i] = recipes[i];
        }
        delete []recipes;
        recipes = new_recipies;
        recipe_capacity = new_capacity;
    }

    Recipe temp;
    temp.setRecipe(name,instructions);
    recipes[recipes->numrecipes++] = temp;
}

