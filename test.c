 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ITEMS 50
#define TAX_RATE 0.05

typedef struct {
    int id;
    char name[50];
    float price;
    char category[20];
} MenuItem;

typedef struct {
    MenuItem *item;
    int quantity;
} OrderItem;

MenuItem menu[MAX_ITEMS];
int menuCount = 0;
OrderItem orders[MAX_ITEMS];
int orderCount = 0;

// Initialize some default menu items
void initializeMenu() {
    // Drinks
    menu[0] = (MenuItem){101, "Coffee", 2.50, "Drinks"};
    menu[1] = (MenuItem){102, "Orange Juice", 3.00, "Drinks"};
    // Main Dishes
    menu[2] = (MenuItem){201, "Burger", 8.99, "Main Dish"};
    menu[3] = (MenuItem){202, "Pizza", 12.50, "Main Dish"};
    // Desserts
    menu[4] = (MenuItem){301, "Ice Cream", 4.75, "Desserts"};
    menuCount = 5;
}

void displayMenu() {
    printf("\n=== MENU ===\n");
    printf("Category: Drinks\n");
    for(int i=0; i<menuCount; i++) {
        if(strcmp(menu[i].category, "Drinks") == 0) {
            printf("%d. %-20s $%.2f\n", menu[i].id, menu[i].name, menu[i].price);
        }
    }

    printf("\nCategory: Main Dish\n");
    for(int i=0; i<menuCount; i++) {
        if(strcmp(menu[i].category, "Main Dish") == 0) {
            printf("%d. %-20s $%.2f\n", menu[i].id, menu[i].name, menu[i].price);
        }
    }

    printf("\nCategory: Desserts\n");
    for(int i=0; i<menuCount; i++) {
        if(strcmp(menu[i].category, "Desserts") == 0) {
            printf("%d. %-20s $%.2f\n", menu[i].id, menu[i].name, menu[i].price);
        }
    }
}

MenuItem* findItem(int id) {
    for(int i=0; i<menuCount; i++) {
        if(menu[i].id == id) {
            return &menu[i];
        }
    }
    return NULL;
}

void placeOrder() {
    int itemId, quantity;
    char choice;

    do {
        displayMenu();
        printf("\nEnter item code: ");
        scanf("%d", &itemId);
        printf("Enter quantity: ");
        scanf("%d", &quantity);

        MenuItem *item = findItem(itemId);
        if(item == NULL) {
            printf("Invalid item code!\n");
            continue;
        }

        orders[orderCount].item = item;
        orders[orderCount].quantity = quantity;
        orderCount++;

        printf("Add another item? (y/n): ");
        scanf(" %c", &choice);
    } while(tolower(choice) == 'y');
}

void generateBill(int saveToFile) {
    float subtotal = 0, tax, total;

    printf("\n=== BILL ===\n");
    printf("%-5s %-20s %-8s %-6s\n", "Qty", "Item", "Price", "Total");

    for(int i=0; i<orderCount; i++) {
        float itemTotal = orders[i].item->price * orders[i].quantity;
        printf("%-5d %-20s $%-7.2f $%-6.2f\n",
              orders[i].quantity,
              orders[i].item->name,
              orders[i].item->price,
              itemTotal);
        subtotal += itemTotal;
    }

    tax = subtotal * TAX_RATE;
    total = subtotal + tax;

    printf("\nSubtotal: $%.2f", subtotal);
    printf("\nTax (5%%): $%.2f", tax);
    printf("\nTotal: $%.2f\n", total);

    if(saveToFile) {
        FILE *file = fopen("bill.txt", "w");
        if(file) {
            fprintf(file, "=== BILL ===\n");
            for(int i=0; i<orderCount; i++) {
                fprintf(file, "%d %s - $%.2f\n",
                       orders[i].quantity,
                       orders[i].item->name,
                       orders[i].item->price * orders[i].quantity);
            }
            fprintf(file, "Total: $%.2f", total);
            fclose(file);
            printf("\nBill saved to bill.txt\n");
        }
    }
}

void adminPanel() {
    int choice;
    printf("\n=== ADMIN PANEL ===\n");
    printf("1. Add Menu Item\n");
    printf("2. Exit\n");  // Simplified for this fix
    printf("Choice: ");
    scanf("%d", &choice);
    getchar(); // Clear newline from buffer

    if(choice == 1) {
        if(menuCount >= MAX_ITEMS) {
            printf("Menu full!\n");
            return;
        }

        // Item Code
        printf("Enter item code (3 digits): ");
        scanf("%d", &menu[menuCount].id);
        getchar(); // Clear buffer

        // Item Name (with spaces)
        printf("Enter item name: ");
        fgets(menu[menuCount].name, 50, stdin);
        menu[menuCount].name[strcspn(menu[menuCount].name, "\n")] = '\0'; // Remove newline

        // Price
        printf("Enter price: ");
        scanf("%f", &menu[menuCount].price);
        getchar(); // Clear buffer

        // Category (with spaces)
        printf("Enter category: ");
        fgets(menu[menuCount].category, 20, stdin);
        menu[menuCount].category[strcspn(menu[menuCount].category, "\n")] = '\0';

        menuCount++;
        printf("Item added successfully!\n");
    }
}

int main() {
    initializeMenu();
    int choice;

    while(1) {
        printf("\n=== RESTAURANT ORDER SYSTEM ===\n");
        printf("1. Customer Menu\n");
        printf("2. Admin Panel\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                placeOrder();
                generateBill(1);
                orderCount = 0; // Reset orders
                break;
            case 2:
                adminPanel();
                break;
            case 3:
                exit(0);
            default:
                printf("Invalid choice!\n");
        }
    }
    return 0;
}
