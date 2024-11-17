#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <direct.h>


#define MAX_FIELD_LENGTH 20
#define MAX_LINE_LENGTH 100
#define MAX_SYMTAB_SIZE 100
#define MAX_OPTAB_SIZE 100

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ------x--------x----------x------------x------ OPTAB ----------------x------------x----------------x-----------x

// Structure for each opcode entry
typedef struct {
    char mnemonic[MAX_FIELD_LENGTH];
    int opcode;
} optab_entry;

// Global array for the opcode table
optab_entry optab[MAX_OPTAB_SIZE];
int optab_size = 0;

// Structure for the opcode lookup table
typedef struct {
    char mnemonic[MAX_FIELD_LENGTH];
    int opcode;
} opcode_mapping;

// Predefined opcode mapping table
opcode_mapping optab_map[] = {
    // Data Transfer Instructions
    {"LDA", 0x00},  // Load accumulator
    {"LDX", 0x04},  // Load index register
    {"LDL", 0x08},  // Load L register
    {"STA", 0x0C},  // Store accumulator
    {"STX", 0x10},  // Store index register
    {"STL", 0x14},  // Store L register
    {"TIX", 0x2C},  // Test index register
    {"LDCH", 0x50}, // Load character to accumulator
    {"STCH", 0x54}, // Store character from accumulator
    {"RMO", 0xAC},  // Register to memory operation

    // Arithmetic Instructions
    {"ADD", 0x18},  // Add
    {"SUB", 0x1C},  // Subtract
    {"MUL", 0x20},  // Multiply
    {"DIV", 0x24},  // Divide
    {"COMP", 0x28}, // Compare
    {"TIXR", 0x2C}, // Test index register (register form)
    {"CLEAR", 0xB4}, // Clear register
    {"TIX", 0x2C},  // Test index register (SIC)
    {"INC", 0x14},  // Increment register

    // Control Instructions
    {"JMP", 0x1F},  // Jump
    {"JSR", 0x20},  // Jump to subroutine
    {"JSP", 0x24},  // Jump to subroutine (indirect)
    {"RET", 0x2C},  // Return
    {"RSUB", 0x4C}, // Return from subroutine
    {"BALR", 0x58}, // Branch and link register
    {"BR", 0x1F},   // Branch (conditional)
    
    // Logical Operations
    {"AND", 0x40},  // Logical AND
    {"OR", 0x44},   // Logical OR
    {"NOT", 0x4C},  // Logical NOT
    {"XOR", 0x60},  // Exclusive OR

    // Input/Output Instructions
    {"RD", 0xD8},   // Read
    {"WD", 0xDC},   // Write
    {"SSK", 0xEC},  // Set switch key

    // Control Flow
    {"BR", 0x1F},   // Branch
    {"BCR", 0x2F},  // Branch conditional
    {"BS", 0x4F},   // Branch and save return address

    // Extended Opcodes (SIC/XE)
    {"LDX", 0x04},  // Load index register (Extended)
    {"LDA", 0x00},  // Load accumulator (Extended)
    {"STA", 0x0C},  // Store accumulator (Extended)
    {"STX", 0x10},  // Store index (Extended)
    {"JMP", 0x1F},  // Jump (Extended)
    {"JSR", 0x20},  // Jump to subroutine (Extended)
    {"SVC", 0xB4},  // Supervisor call
    {"RSUB", 0x4C}, // Return from subroutine (Extended)
    {"TIX", 0x2C},  // Test index register (SIC)

    // Program Control Instructions (SIC/XE)
    {"CSECT", 0x00}, // Control section
    {"END", 0x00},   // End of program

    // Miscellaneous Instructions
    {"NOP", 0x00},   // No operation
    {"HALT", 0x00},  // Halt
    {"WAIT", 0x00},  // Wait (halt)

    // More instructions can be added
};


// Function to add an entry to the OPTAB
void addToOptab(const char *mnemonic, int opcode) {
    // Check if the mnemonic is already in OPTAB
    for (int i = 0; i < optab_size; i++) {
        if (strcmp(optab[i].mnemonic, mnemonic) == 0) {
            return;  // If it exists, do nothing
        }
    }

    // Add new entry to OPTAB if space is available
    if (optab_size < MAX_OPTAB_SIZE) {
        strcpy(optab[optab_size].mnemonic, mnemonic);
        optab[optab_size].opcode = opcode;
        optab_size++;
    } else {
        printf("OPTAB is full, cannot add more entries.\n");
    }
}

// Function to write the OPTAB to a file (optab.txt)
void writeOptabToFile() {
    FILE *optab_file = fopen("optab.txt", "w");
    if (!optab_file) {
        perror("Error opening optab.txt for writing");
        return;
    }


    for (int i = 0; i < optab_size; i++) {
        fprintf(optab_file, "%-10s %-10X\n", optab[i].mnemonic, optab[i].opcode);
    }

    fclose(optab_file);
    printf("Successfully written to the Optab.\n");
}

// Function to get the opcode for a given mnemonic from the mapping table
int getOpcode(const char *mnemonic) {
    // Search through the opcode mapping table
    for (int i = 0; i < sizeof(optab_map) / sizeof(optab_map[0]); i++) {
        if (strcmp(optab_map[i].mnemonic, mnemonic) == 0) {
            return optab_map[i].opcode;  // Return the opcode if found
        }
    }
    return -1;  // Return -1 if mnemonic is not found
}

// ------x--------x----------x------------x------ OPTAB ----------------x------------x----------------x-----------x
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX



// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ------x--------x----------x------------x------ SYMTAB ----------------x------------x----------------x-----------x

// Structure for each symbol table entry to store symbol and it's address
typedef struct{
    char symbol[MAX_FIELD_LENGTH];
    int address;
}symbtab_entry;


// An array of structures for the symbol table
symbtab_entry symtab[MAX_SYMTAB_SIZE];
int symtab_size = 0;


// Function to search for a label in the symbol table
int searchSymtab(const char* symbol){

    // Check and compare the current symbol with every symbol in the symtab
    for(int i=0;i<symtab_size;i++){
        if(!strcmp(symtab[i].symbol, symbol)) return i; // Return the index if the symbol is found
    }

    // Return -1 if the symbol is not found
    return -1;
}


// Function to write to the symbol table
void addToSymtab(const char* symbol, int address){

    // If the symbol is already in the symbtab, return error
    if(searchSymtab(symbol) != -1){
        fprintf(stderr, "Error: Duplicate symbol '%s'.\n", symbol);
        return;
    }

    // If the size of the symbtab maxes out
    if(symtab_size >= MAX_SYMTAB_SIZE){
        fprintf(stderr, "Error: Symbol table is full.\n");
        return;
    }

    // Insert the symbol into the symtab, along with its address
    strcpy(symtab[symtab_size].symbol, symbol);
    symtab[symtab_size].address = address;
    symtab_size++;
}


// Fucntion to write the symbol table to a text file
void writeSymtabToFile() {
    FILE* symtab_file = fopen("symtab.txt", "w");
    if (!symtab_file) {
        perror("Error opening symtab.txt");
        return;
    }


    for (int i = 0; i < symtab_size; i++) {
        fprintf(symtab_file, "%-10s %04X\n", symtab[i].symbol, symtab[i].address);
    }

    fclose(symtab_file);
    printf("Successfully written to the Symtab.\n");
}

// ------x--------x----------x------------x------ SYMTAB ----------------x------------x----------------x-----------x
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ------x--------x----------x------------x------ INTERMEDIATE ----------------x------------x----------x-----------x
// Structure for each instruction, to store label, mnemonic and operand
typedef struct {
    char label[MAX_FIELD_LENGTH];
    char mnemonic[MAX_FIELD_LENGTH];
    char operand[MAX_FIELD_LENGTH];
} source_line;



// Function to get the instruction size
int getInstructionSize(const char *mnemonic, const char *operand) {
    // WORD -> 3 size
    // RESW -> 3 * sizeof(operand)
    // RESB -> sizeof(operand)
    // Byte: 
    //     starts with "C" then it's a char -> length of string inside the quotes
    //     starts with "X" then it's a hexa -> hexadecimal bytes
    // Default size of instruction for SIC -> 3

    if (!strcmp(mnemonic, "WORD")) return 3;
    else if (!strcmp(mnemonic, "RESW")) return 3 * atoi(operand);
    else if (!strcmp(mnemonic, "RESB")) return atoi(operand);
    else if (!strcmp(mnemonic, "BYTE")) {
        if (operand[0] == 'C') return strlen(operand) - 3; // Length of string inside quotes
        else if (operand[0] == 'X') return (strlen(operand) - 3) / 2; // Hexadecimal bytes
    }
    else return 3;

    return 0;
}

// ------x--------x----------x------------x------ INTERMEDIATE ----------------x------------x----------------x-----------x
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX



void saveFiles() {
    // Create the "Pass2" directory if it doesn't exist
    if (_mkdir("E:/SIC-Assembler/Pass2") == -1) {
        if (errno != EEXIST) {  // If it's not "already exists" error, print error
            perror("Error creating directory");
            return;
        }
    }

    // Define the source files in the Pass1 folder (relative paths)
    char sourceFile[] = "source.txt";
    char intermediateFile[] = "intermediate.txt";
    char symtabFile[] = "symtab.txt";
    char optabFile[] = "optab.txt";

    

    // Define the target files in the Pass2 folder (relative paths)
    char sourceOutFile[] = "E:/SIC-Assembler/Pass2/source.txt";
    char intermediateOutFile[] = "E:/SIC-Assembler/Pass2/intermediate.txt";
    char symtabOutFile[] = "E:/SIC-Assembler/Pass2/symtab.txt";
    char optabOutFile[] = "E:/SIC-Assembler/Pass2/optab.txt";

    // Open the source files for reading
    FILE *source_in = fopen(sourceFile, "r");
    FILE *intermediate_in = fopen(intermediateFile, "r");
    FILE *symtab_in = fopen(symtabFile, "r");
    FILE *optab_in = fopen(optabFile, "r");

    if (source_in == NULL || intermediate_in == NULL || symtab_in == NULL || optab_in == NULL) {
        perror("Error opening one or more source files for reading");
        return;
    }

    // Open the target files for writing
    FILE *source_out = fopen(sourceOutFile, "w");
    FILE *intermediate_out = fopen(intermediateOutFile, "w");
    FILE *symtab_out = fopen(symtabOutFile, "w");
    FILE *optab_out = fopen(optabOutFile, "w");

    if (source_out == NULL || intermediate_out == NULL || symtab_out == NULL || optab_out == NULL) {
        perror("Error opening one or more target files for writing");
        return;
    }

    // Example: Copying content from Pass1 files to Pass2 files
    char ch;
    while ((ch = fgetc(source_in)) != EOF) {
        fputc(ch, source_out);
    }
    while ((ch = fgetc(intermediate_in)) != EOF) {
        fputc(ch, intermediate_out);
    }
    while ((ch = fgetc(symtab_in)) != EOF) {
        fputc(ch, symtab_out);
    }
    while ((ch = fgetc(optab_in)) != EOF) {
        fputc(ch, optab_out);
    }

    // Close the files
    fclose(source_in);
    fclose(intermediate_in);
    fclose(symtab_in);
    fclose(optab_in);

    fclose(source_out);
    fclose(intermediate_out);
    fclose(symtab_out);
    fclose(optab_out);

    printf("Files saved in 'Pass2' directory successfully.\n");
}



// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ------x--------x----------x------------x------ MAIN ----------------x------------x----------------x-----------x

int main() {
    // A pointer for the source file
    // line is a character array for each instruction in the source file
    // current_line is a structure to store label, mnemonic and operand for the current instruction
    FILE *source_file, *intermediate_file;
    char line[MAX_LINE_LENGTH];
    source_line current_line;

    // Initialising the location counter -> locctr
    int locctr = 0;

    // We will get the start address based on the start directive
    // but if start directive is not there, then we'll assume the start address to be 0
    int start_address = 0, is_start_found = 0;

    // Opening the source file
    source_file = fopen("source.txt", "r");
    if (!source_file) {
        perror("Error opening the file.");
        return EXIT_FAILURE;
    }

    // Opening the intermediate file
    intermediate_file = fopen("intermediate.txt", "w");
    if (!intermediate_file) {
        perror("Error opening the file.");
        return EXIT_FAILURE;
    }

    // Printing table header
    printf("%-10s %-10s %-10s %-10s\n", "LOCCTR", "Label", "Mnemonic", "Operand");
    printf("------------------------------------------------------\n");

    // Reading through the source file
    printf("Reading the source file.\n");
    while (fgets(line, sizeof(line), source_file)) {
        // Remove the newline character from the end of the line
        line[strcspn(line, "\n")] = 0;

        // Initialising label, mnemonic and operand as empty strings
        // for the current instruction 
        strcpy(current_line.label, "");
        strcpy(current_line.mnemonic, "");
        strcpy(current_line.operand, "");

        // If the instruction does not start with a tab or space then it has all three
        // label, mnemonic and operand
        if (line[0] != ' ' && line[0] != '\t') {
            sscanf(line, "%s %s %s", current_line.label, current_line.mnemonic, current_line.operand);
        } else {
            // If the line starts with tab or space it only has mnemonic and operand
            sscanf(line, "%s %s", current_line.mnemonic, current_line.operand);
        }


        // If the instruction has a label
        if (strlen(current_line.label) > 0) { 
            addToSymtab(current_line.label, locctr);
        }

        // Add the mnemonic to the OPTAB if it is not already there
        if (strlen(current_line.mnemonic) > 0) {
            // Get the opcode for the mnemonic
            int opcode = getOpcode(current_line.mnemonic);
            if (opcode != -1) {
                addToOptab(current_line.mnemonic, opcode);  // Add to OPTAB if opcode found
            }
        }

        // Handling the start directive
        if (!strcmp(current_line.mnemonic, "START")) {
            // Assign the starting address to locctr
            locctr = strtol(current_line.operand, NULL, 16);

            // If start address is found assign it to start_address
            // and is_start_found becomes true
            start_address = locctr;
            is_start_found = 1;

            // Print the first instruction with LOCCTR
            printf("%-10X %-10s %-10s %-10s\n", locctr, current_line.label, current_line.mnemonic, current_line.operand);
            
            // Write the instruction in the intermediate file
            fprintf(intermediate_file, "%-10X %-10s %-10s %-10s\n", locctr, current_line.label, current_line.mnemonic, current_line.operand);

            // No further computing needed for starting address
            continue;
        }

        // Print the LOCCTR and current instruction
        printf("%-10X %-10s %-10s %-10s\n", locctr, current_line.label, current_line.mnemonic, current_line.operand);

        // Write the instruction in the intermediate file
        fprintf(intermediate_file, "%-10X %-10s %-10s %-10s\n", locctr, current_line.label, current_line.mnemonic, current_line.operand);

        // Increment the locctr according to the instruction size
        int increment = getInstructionSize(current_line.mnemonic, current_line.operand);

        // Check for any error in getting the size
        if (increment == 0) {
            fprintf(stderr, "Error: Unknown mnemonic '%s' on line: %s\n", current_line.mnemonic, line);
        }

        locctr += increment;
    }

    // Finalize
    if (!is_start_found) {
        fprintf(stderr, "Warning: No START directive found. LOCCTR starts from 0.\n");
    }
    printf("------------------------------------------------------\n");
    printf("Starting Address: %04X\n", start_address);
    printf("End Address: %04X\n", locctr);
    printf("Program Length: %04X\n", locctr - start_address);

    // Write the symtab to the symtab.txt & optab to optab.txt
    writeSymtabToFile();
    writeOptabToFile();


    // Closing the source & intermediate file
    fclose(source_file);
    fclose(intermediate_file);


    // Save the files in the Pass2 folder as well
    saveFiles();

    return 0;
}


// ------x--------x----------x------------x------ MAIN ----------------x------------x----------------x-----------x
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
