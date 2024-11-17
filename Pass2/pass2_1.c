#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 100
#define MAX_SYMBOLS 50
#define MAX_OPCODES 50

// Define structures for symbol table and opcode table
struct Symbol
{
    char name[10];
    char address[10];
};

struct Opcode
{
    char mnemonic[10];
    char code[10];
};

// Function to parse the symbol table
int parseSymbolTable(FILE *symtabFile, struct Symbol symbols[])
{
    int count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), symtabFile))
    {
        sscanf(line, "%s %s", symbols[count].name, symbols[count].address);
        count++;
    }
    return count;
}

// Function to parse the opcode table
int parseOpcodeTable(FILE *optabFile, struct Opcode opcodes[])
{
    int count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), optabFile))
    {
        sscanf(line, "%s %s", opcodes[count].mnemonic, opcodes[count].code);
        count++;
    }
    return count;
}

// Function to parse the intermediate file
int parseIntermediate(FILE *intermediateFile, char intermediate[][3][MAX_LINE_LENGTH])
{
    int count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), intermediateFile))
    {
        sscanf(line, "%s %s %s", intermediate[count][0], intermediate[count][1], intermediate[count][2]);
        count++;
    }
    return count;
}

// Function to look up the symbol address
char *lookupSymbol(struct Symbol symbols[], int symbolCount, const char *name)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbols[i].name, name) == 0)
        {
            return symbols[i].address;
        }
    }
    return NULL;
}

// Function to look up the opcode machine code
char *lookupOpcode(struct Opcode opcodes[], int opcodeCount, const char *mnemonic)
{
    for (int i = 0; i < opcodeCount; i++)
    {
        if (strcmp(opcodes[i].mnemonic, mnemonic) == 0)
        {
            return opcodes[i].code;
        }
    }
    return NULL;
}

int main()
{
    FILE *intermediateFile = fopen("intermediate.txt", "r");
    FILE *symtabFile = fopen("symtab.txt", "r");
    FILE *optabFile = fopen("optab.txt", "r");
    FILE *objectProgramFile = fopen("object_program.txt", "w");

    if (!intermediateFile || !symtabFile || !optabFile || !objectProgramFile)
    {
        printf("Error opening files.\n");
        return 1;
    }

    struct Symbol symbols[MAX_SYMBOLS];
    struct Opcode opcodes[MAX_OPCODES];
    char intermediate[MAX_SYMBOLS][3][MAX_LINE_LENGTH]; // To store intermediate records

    int symbolCount = parseSymbolTable(symtabFile, symbols);
    int opcodeCount = parseOpcodeTable(optabFile, opcodes);
    int intermediateCount = parseIntermediate(intermediateFile, intermediate);

    // Write the header record
    fprintf(objectProgramFile, "H^\tFIRST^\t%s\t^%s\n", intermediate[0][0], intermediate[intermediateCount - 1][0]);

    // Write the text records
    fprintf(objectProgramFile, "T^\t%s^", intermediate[0][0]);
    for (int i = 0; i < intermediateCount; i++)
    {
        char *opcode = lookupOpcode(opcodes, opcodeCount, intermediate[i][1]);
        if (opcode)
        {
            // Lookup the operand in the symbol table if present
            char *operandAddress = lookupSymbol(symbols, symbolCount, intermediate[i][2]);
            if (operandAddress)
            {
                // Combine opcode with operand address
                fprintf(objectProgramFile, "\t%s%s\t", opcode, operandAddress);
            }
            else
            {
                // If operand address not found, handle accordingly (e.g., use 0)
                fprintf(objectProgramFile, "%s0000", opcode);
            }
        }
    }
    fprintf(objectProgramFile, "\n");

    // Write the end record
    fprintf(objectProgramFile, "E^\t%s\n", intermediate[0][0]);

    // Close all files
    fclose(intermediateFile);
    fclose(symtabFile);
    fclose(optabFile);
    fclose(objectProgramFile);

    printf("Object program generated successfully!\n");
    return 0;
}
