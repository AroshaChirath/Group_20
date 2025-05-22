#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ITEMS 50
#define TAX_RATE 0.05
#define MENU_FILENAME "menu.txt" // Define the menu file name

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

// Function to save menu items to a file
void saveMenuToFile() {
    FILE *file = fopen(MENU_FILENAME, "w");
    if (file == NULL) {
        printf("Error: Could not open %s for writing.\n", MENU_FILENAME);
        return;
    }

    for (int i = 0; i < menuCount; i++) {
        fprintf(file, "%d,%s,%.2f,%s\n", menu[i].id, menu[i].name, menu[i].price, menu[i].category);
    }

    fclose(file);
    printf("Menu saved to %s successfully.\n", MENU_FILENAME);
}

// Function to load menu items from a file
void loadMenuFromFile() {
    FILE *file = fopen(MENU_FILENAME, "r");
    if (file == NULL) {
        printf("No existing menu file found. Starting with default menu.\n");
        // If file doesn't exist, initialize default menu
        // Drinks
        menu[0] = (MenuItem){101, "Coffee", 2.50, "Drinks"};
        menu[1] = (MenuItem){102, "Orange Juice", 3.00, "Drinks"};
        // Main Dishes
        menu[2] = (MenuItem){201, "Burger", 8.99, "Main Dish"};
        menu[3] = (MenuItem){202, "Pizza", 12.50, "Main Dish"};
        // Desserts
        menu[4] = (MenuItem){301, "Ice Cream", 4.75, "Desserts"};
        menuCount = 5;
        saveMenuToFile(); // Save the default menu to the file
        return;
    }

    menuCount = 0; // Reset menu count before loading
    char line[100];
    while (fgets(line, sizeof(line), file) != NULL && menuCount < MAX_ITEMS) {
        // Parse the line: id,name,price,category
        char *token;
        char *rest = line;

        // ID
        token = strtok(rest, ",");
        if (token == NULL) continue;
        menu[menuCount].id = atoi(token);

        // Name
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(menu[menuCount].name, token, sizeof(menu[menuCount].name) - 1);
        menu[menuCount].name[sizeof(menu[menuCount].name) - 1] = '\0';

        // Price
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        menu[menuCount].price = atof(token);

        // Category (remove newline character)
        token = strtok(NULL, "\n"); // Use newline as delimiter for the last token
        if (token == NULL) continue;
        strncpy(menu[menuCount].category, token, sizeof(menu[menuCount].category) - 1);
        menu[menuCount].category[sizeof(menu[menuCount].category) - 1] = '\0';

        menuCount++;
    }
    fclose(file);
    printf("Menu loaded from %s successfully.\n", MENU_FILENAME);
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
    printf("\n"); // Add a newline for better formatting
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
        printf("Enter item code: ");
        scanf("%d", &itemId);
        printf("Enter quantity: ");
        scanf("%d", &quantity);

        MenuItem *item = findItem(itemId);
        if(item == NULL) {
            printf("Invalid item code! Please try again.\n");
            continue;
        }

        if (orderCount >= MAX_ITEMS) {
            printf("Order list is full. Cannot add more items.\n");
            break;
        }

        orders[orderCount].item = item;
        orders[orderCount].quantity = quantity;
        orderCount++;
        printf("%s added to order.\n", item->name);

        printf("Add another item? (y/n): ");
        scanf(" %c", &choice); // Space before %c to consume newline
    } while(tolower(choice) == 'y');
}

void generateBill(int saveToFile) {
    float subtotal = 0, tax, total;

    if (orderCount == 0) {
        printf("\nNo items in the current order to generate a bill.\n");
        return;
    }

    printf("\n=== BILL ===\n");
    printf("%-5s %-20s %-8s %-9s\n", "Qty", "Item", "Price", "Total"); // Adjusted column width

    for(int i=0; i<orderCount; i++) {
        float itemTotal = orders[i].item->price * orders[i].quantity;
        printf("%-5d %-20s $%-7.2f $%-8.2f\n",
             orders[i].quantity,
             orders[i].item->name,
             orders[i].item->price,
             itemTotal);
        subtotal += itemTotal;
    }

    tax = subtotal * TAX_RATE;
    total = subtotal + tax;

    printf("------------------------------------------\n"); // Separator
    printf("Subtotal:            $%.2f\n", subtotal);
    printf("Tax (5%%):            $%.2f\n", tax);
    printf("Total:               $%.2f\n", total);
    printf("------------------------------------------\n");

    if(saveToFile) {
        FILE *file = fopen("bill.txt", "a"); // Use "a" to append to the bill.txt
        if(file) {
            fprintf(file, "\n--- New Bill ---\n"); // Add a separator for new bills
            fprintf(file, "%-5s %-20s %-8s %-9s\n", "Qty", "Item", "Price", "Total");
            for(int i=0; i<orderCount; i++) {
                fprintf(file, "%-5d %-20s $%-7.2f $%-8.2f\n",
                                 orders[i].quantity,
                                 orders[i].item->name,
                                 orders[i].item->price,
                                 orders[i].item->price * orders[i].quantity);
            }
            fprintf(file, "Subtotal:            $%.2f\n", subtotal);
            fprintf(file, "Tax (5%%):            $%.2f\n", tax);
            fprintf(file, "Total:               $%.2f\n", total);
            fclose(file);
            printf("\nBill details appended to bill.txt\n");
        } else {
            printf("Error: Could not open bill.txt for writing.\n");
        }
    }
}

void adminPanel() {
    int choice;
    printf("\n=== ADMIN PANEL ===\n");
    printf("1. Add New Menu Item\n");
    printf("2. Save Menu to File\n"); // New option
    printf("3. Exit Admin Panel\n");
    printf("Choice: ");
    scanf("%d", &choice);
    getchar(); // Clear newline from buffer

    switch (choice) {
        case 1:
            if(menuCount >= MAX_ITEMS) {
                printf("Menu full! Cannot add more items.\n");
                return;
            }

            printf("Enter item code (e.g., 101, 205): ");
            scanf("%d", &menu[menuCount].id);
            getchar(); // Clear buffer

            printf("Enter item name: ");
            fgets(menu[menuCount].name, sizeof(menu[menuCount].name), stdin);
            menu[menuCount].name[strcspn(menu[menuCount].name, "\n")] = '\0'; // Remove newline

            printf("Enter price: ");
            scanf("%f", &menu[menuCount].price);
            getchar(); // Clear buffer

            printf("Enter category (e.g., Drinks, Main Dish, Desserts): ");
            fgets(menu[menuCount].category, sizeof(menu[menuCount].category), stdin);
            menu[menuCount].category[strcspn(menu[menuCount].category, "\n")] = '\0';

            menuCount++;
            printf("Item added successfully!\n");
            saveMenuToFile(); // Save after adding a new item
            break;
        case 2:
            saveMenuToFile();
            break;
        case 3:
            printf("Exiting Admin Panel.\n");
            break;
        default:
            printf("Invalid choice!\n");
            break;
    }
}

int main() {
    loadMenuFromFile(); // Load menu items at the start
    int choice;

    while(1) {
        printf("\n=== RESTAURANT ORDER SYSTEM ===\n");
        printf("1. Customer Menu (Place Order & Generate Bill)\n");
        printf("2. Admin Panel\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                placeOrder();
                generateBill(1); // Pass 1 to save the bill to file
                orderCount = 0; // Reset orders for the next customer
                break;
            case 2:
                adminPanel();
                break;
            case 3:
                saveMenuToFile(); // Save menu before exiting
                printf("Thank you for using the system. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
    return 0;
}
