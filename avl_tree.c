#include<stdlib.h>
#include<stdbool.h>
#include<stdio.h>
#include<string.h>
typedef struct {
    int key;
    } element;
typedef struct treeNode *treePointer;
struct treeNode {
    treePointer leftChild;
    element data;
    short int bf;
    treePointer rightChild;
};  
typedef struct QueueNode {
    treePointer node;
    int index;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
} Queue;

void enqueue(Queue *q, treePointer node, int index) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    newNode->node = node;
    newNode->index = index;
    newNode->next = NULL;

    if (q->rear) {
        q->rear->next = newNode;
    } else {
        q->front = newNode;
    }
    q->rear = newNode;
}

QueueNode *dequeue(Queue *q) {
    if (!q->front) return NULL;

    QueueNode *temp = q->front;
    q->front = q->front->next;
    if (!q->front) {
        q->rear = NULL;
    }
    return temp;
}

bool isQueueEmpty(Queue *q) {
    return q->front == NULL;
}

void levelOrderTraversal(treePointer root) {
    if (!root) {
        printf("-\n");
        return;
    }

    Queue q = {NULL, NULL};
    enqueue(&q, root, 1); // Start with index 1

    int lastValidIndex = 0; // Tracks the last valid (non-NULL) node index
    QueueNode *lastValidNode = NULL;

    // Perform level-order traversal
    while (!isQueueEmpty(&q)) {
        QueueNode *current = dequeue(&q);

        if (current->node) {
            lastValidIndex = current->index; // Update last valid index
            enqueue(&q, current->node->leftChild, current->index * 2);
            enqueue(&q, current->node->rightChild, current->index * 2 + 1);
        }

        if (!lastValidNode || current->index <= lastValidIndex) {
            lastValidNode = current;
        }

        free(current); // Free the queue node
    }

    // Reset queue for second pass, printing only up to `lastValidIndex`
    enqueue(&q, root, 1);
    int currentIndex = 0;

    while (!isQueueEmpty(&q)) {
        QueueNode *current = dequeue(&q);

        if (current->index > lastValidIndex) {
            free(current);
            break; // Stop printing once past last valid index
        }

        currentIndex++;

        if (current->node) {
            printf("%d", current->node->data.key);
            enqueue(&q, current->node->leftChild, current->index * 2);
            enqueue(&q, current->node->rightChild, current->index * 2 + 1);
        } else {
            printf("-");
        }

        printf((currentIndex < lastValidIndex) ? ", " : "\n");
        free(current); // Free the queue node
    }
}



void rightRotation(treePointer * parent, int *unbalanced);
void leftRotation(treePointer * parent, int *unbalanced);
void deleteNode(treePointer *parent, int key, int *unbalanced) {
    if (!*parent) { 
        // Step 1: Base case - key not found
        // If the tree/subtree is empty, the key does not exist.
        printf("Key %d not found in the tree.\n", key);
        return;
    }

    if (key < (*parent)->data.key) {
        // Step 2: Traverse the left subtree
        // If the key is smaller than the current node's key, go to the left subtree.
        deleteNode(&(*parent)->leftChild, key, unbalanced);

        // Step 6: After deletion, check if the current subtree is unbalanced
        if (*unbalanced) {
            // Adjust the balance factor after removing from the left subtree
            switch ((*parent)->bf) {
                case 1:  // Left subtree height decreased, now balanced
                    (*parent)->bf = 0;
                    break;
                case 0:  // Left subtree height decreased, now right-heavy
                    (*parent)->bf = -1;
                    *unbalanced = false; // Tree is still balanced
                    break;
                case -1: // Tree becomes unbalanced, requires right rotation
                    rightRotation(parent, unbalanced);
                    break;
            }
        }
    } else if (key > (*parent)->data.key) {
        // Step 3: Traverse the right subtree
        // If the key is larger than the current node's key, go to the right subtree.
        deleteNode(&(*parent)->rightChild, key, unbalanced);

        // Step 6: After deletion, check if the current subtree is unbalanced
        if (*unbalanced) {
            // Adjust the balance factor after removing from the right subtree
            switch ((*parent)->bf) {
                case -1: // Right subtree height decreased, now balanced
                    (*parent)->bf = 0;
                    break;
                case 0:  // Right subtree height decreased, now left-heavy
                    (*parent)->bf = 1;
                    *unbalanced = false; // Tree is still balanced
                    break;
                case 1:  // Tree becomes unbalanced, requires left rotation
                    leftRotation(parent, unbalanced);
                    break;
            }
        }
    } else {
        // Step 4: Node to delete is found
        if ((*parent)->leftChild && (*parent)->rightChild) {
            // Case 4a: Node has two children
            // Find the in-order successor (smallest value in the right subtree).
            treePointer successor = (*parent)->rightChild;
            while (successor->leftChild) {
                successor = successor->leftChild;
            } 
            // Replace the current node's data with the successor's data.
            (*parent)->data = successor->data;

            // Recursively delete the successor node from the right subtree.
            deleteNode(&(*parent)->rightChild, successor->data.key, unbalanced);

            // Step 6: After removing the successor, check balance factor
            if (*unbalanced) {
                switch ((*parent)->bf) {
                    case -1:
                        (*parent)->bf = 0;
                        break;
                    case 0:
                        (*parent)->bf = 1;
                        *unbalanced = false;
                        break;
                    case 1:
                        leftRotation(parent, unbalanced);
                        break;
                }
            }
        } else {
            // Case 4b: Node has one or no children
            treePointer temp = *parent;

            // Replace the current node with its non-null child (or NULL if no child exists).
            if ((*parent)->leftChild) {
                *parent = (*parent)->leftChild;
            } else {
                *parent = (*parent)->rightChild;
            }

            free(temp); // Free the memory of the deleted node
            *unbalanced = true; // Signal that the subtree height has decreased
        }
    }
}

void avlInsert(treePointer *parent, element x, int *unbalanced){
    if(!*parent){ //insert element into null tree;
        *unbalanced = true;
        *parent = malloc(sizeof(struct treeNode));
        (*parent)-> leftChild = (*parent)->rightChild = NULL;(*parent) -> bf = 0;
        (*parent)-> data = x;
    }else if(x.key < (*parent) -> data.key){
        avlInsert(&(*parent) -> leftChild, x, unbalanced);
        if ( * unbalanced) {
            //left brach has grown higher;
            switch((*parent) -> bf) {
                case -1 : 
                    (*parent) -> bf = 0;
                    *unbalanced = false;
                    break;
                case 0:
                    (*parent) -> bf = 1;
                    break;
                case 1:
                    leftRotation(parent,unbalanced);
            }
        }
    }else if(x.key > (*parent) -> data.key){
        avlInsert(&(*parent)->rightChild, x, unbalanced);
        if(*unbalanced){
            //right brach has grown higher;
            switch((*parent) -> bf) {
                case 1 : 
                    (*parent) -> bf = 0;
                    *unbalanced = false;
                    break;
                case 0 :
                    (*parent) -> bf = -1;
                    break;
                case -1:
                    rightRotation(parent, unbalanced);
            }
        }
    }else{
        *unbalanced = false;
        printf("The key is already in the tree");
    }
}
void leftRotation(treePointer * parent, int *unbalanced){
    treePointer grandChild, child;
    child = (*parent) ->leftChild;
    if(child ->bf == 1){
        // LL rotation
        (*parent) -> leftChild = child -> rightChild;
        child->rightChild = *parent;
        (*parent) -> bf = 0;
        (*parent) = child;
    }else {
        //LR rotation
        grandChild = child -> rightChild;
        child -> rightChild = grandChild -> leftChild;
        grandChild -> leftChild = child;
        (*parent) -> leftChild = grandChild -> rightChild;
        grandChild -> rightChild = *parent;
        switch(grandChild -> bf){
            case 1:
                (*parent) -> bf = -1;
                child-> bf = 0;
                break;
            case 0:
                (*parent) -> bf = 0;
                child-> bf = 0;
                break;
            case -1:
                (*parent) -> bf = 0;
                child -> bf = 1;
        }
        *parent = grandChild;
    }
    (*parent) -> bf = 0;
    *unbalanced = false;
}
void rightRotation(treePointer * parent, int *unbalanced){
    treePointer grandChild, child;
    child = (*parent) ->rightChild;
    if(child ->bf == -1){
        // RR rotation
        (*parent) -> rightChild = child -> leftChild;
        child->leftChild = *parent;
        (*parent) -> bf = 0;
        (*parent) = child;
    }else {
        //RL rotation
        grandChild = child -> leftChild;
        child -> leftChild = grandChild -> rightChild;
        grandChild -> rightChild = child;
        (*parent) -> rightChild = grandChild -> leftChild;
        grandChild -> leftChild = *parent;
        switch(grandChild -> bf){
            case 1:
                (*parent) -> bf = -1;
                child-> bf = 0;
                break;
            case 0:
                (*parent) -> bf = 0;
                child->bf = 0;
                break;
            case -1:
                (*parent) -> bf = 0;
                child -> bf = 1;
        }
        *parent = grandChild;
    }
    (*parent) -> bf = 0;
    *unbalanced = false;
}

void inOrderTraversal(treePointer root) {
    if(root != NULL) {
        inOrderTraversal(root->leftChild);
        printf("%d ", root->data.key);
        inOrderTraversal(root->rightChild);
    }
}

int main() {
    treePointer root = NULL; // Initialize the AVL tree root
    int unbalanced = false;  // Flag to track if the tree becomes unbalanced
    element newElement;      // Variable to hold the element to be inserted or deleted
    int choice;
    char command[10];        // To hold the user command ("insert", "delete", "print")

    printf("AVL Tree Operations\n");
    printf("You can perform insertions or deletions.\n");
    printf("Type 'insert' to add keys, 'delete' to remove keys, 'print' to view the tree. Enter 'exit' to stop.\n");

    while (1) {
        printf("\nPlease choose a command (insert/delete/print/exit): ");
        fflush(stdout);
        scanf("%s", command); // Read the user's command

        if (strcmp(command, "insert") == 0) {
            printf("Enter a key to insert (-1 to stop): ");
            fflush(stdout);
            scanf("%d", &choice);

            if (choice == -1) { // Exit condition for insertion
                break;
            }

            newElement.key = choice;
            avlInsert(&root, newElement, &unbalanced);

            printf("Key %d inserted successfully!\n", choice);

        } else if (strcmp(command, "delete") == 0) {
            printf("Enter a key to delete: ");
            fflush(stdout);
            scanf("%d", &choice);

            deleteNode(&root, choice, &unbalanced);

            printf("Key %d deleted successfully (if it existed)!\n", choice);

        } else if (strcmp(command, "print") == 0) {
            printf("In-order traversal of the AVL tree:\n");
            inOrderTraversal(root); // Print the current tree structure
            printf("\n");

            printf("Level order traversal of the AVL tree:\n");
            levelOrderTraversal(root); // Print the tree in level order
            printf("\n");

        } else if (strcmp(command, "exit") == 0) {
            printf("Exiting the program.\n");
            break;

        } else {
            printf("Invalid command. Please type 'insert', 'delete', 'print', or 'exit'.\n");
        }
    }

    return 0;
}
