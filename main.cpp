#include <iostream>
using namespace std;

class Ingredient{
public:
    string name;
    int quantity;
    string quantity_type;
    Ingredient(){}
    Ingredient(string name) {
        this->name = name;
    }
};

class Recipe {
private:
    string name;
    Ingredient* ingredients;
    string instructions;
    int capacity;
    static int numrecipes;
public:
    Recipe() {
        cout <<"Recipe name: "<<endl;
        getline(cin,name);
        capacity = 10;
        ingredients = new Ingredient[capacity];
        numrecipes++;
    }
    Recipe(string name) {
        this->name = name;
        capacity = 10;
        ingredients = new Ingredient[capacity];
        numrecipes++;
    }
    void setIngredient(Ingredient* ingred) {

        for (int i = 0; i < capacity; i++) {
            ingredients[i].name = ingred->name;
            ingredients[i].quantity = ingred->quantity;
            ingredients[i].quantity_type = ingred->quantity_type;
        }
    }
    void setIngredient(){
            for (int i = 0; i < capacity; i++) {
                cout<<"Ingredient "<<i<<" :";
                getline(cin,ingredients[i].name);
                cout<<"Quantity:";
                cin >> ingredients[i].quantity;
                cout<<"Quantity type (tablespoons / teaspoons / cups etc) :";
                getline(cin,ingredients[i].quantity_type);
            }
    }

    ~Recipe() {
        delete []ingredients;
        numrecipes--;
    }
};
int Recipe::numrecipes = 0;

class RecipeManager{
    Recipe *recipes;
    int recipe_capacity;

public:
    RecipeManager() {
        recipe_capacity = 10;
        recipes = new Recipe[recipe_capacity];
    }

    //Function for user to input their own recipes
    void addRecipe() {
    }
    ~RecipeManager() {
        delete []recipes;
    }
};


int main() {

}