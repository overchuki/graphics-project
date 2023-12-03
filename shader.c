/*
 *   This is a c file that contains all shader related functions. Taken directly from ex25
 */

#include "CSCIx229.h"
#include "igor.h"

// Read text file
char* ReadText(char *file) {
    char* buffer;
    //  Open file
    FILE* f = fopen(file,"rt");
    if (!f) Fatal("Cannot open text file %s\n",file);
    //  Seek to end to determine size, then rewind
    fseek(f,0,SEEK_END);
    int n = ftell(f);
    rewind(f);
    //  Allocate memory for the whole file
    buffer = (char*)malloc(n+1);
    if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
    //  Snarf the file
    int readNum = fread(buffer,n,1,f);
    // printf("%d: %s\n", readNum, buffer);
    if (readNum!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
    buffer[n] = 0;
    //  Close and return
    fclose(f);
    return buffer;
}

// Print Shader Log
void PrintShaderLog(int obj, char* file) {
    int len = 0;
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &len);
    if (len > 1) {
        int n = 0;
        char* buffer = (char *)malloc(len);
        if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n", len);
        glGetShaderInfoLog(obj, len, &n, buffer);
        fprintf(stderr, "%s:\n%s\n", file, buffer);
        free(buffer);
    }
    glGetShaderiv(obj, GL_COMPILE_STATUS, &len);
    if (!len) Fatal("Error compiling %s\n",file);
}

// Print Program Log
void PrintProgramLog(int obj) {
    int len = 0;
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &len);
    if (len > 1) {
        int n = 0;
        char* buffer = (char *)malloc(len);
        if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n", len);
        glGetProgramInfoLog(obj, len, &n, buffer);
        fprintf(stderr, "%s\n", buffer);
    }
    glGetProgramiv(obj, GL_LINK_STATUS, &len);
    if (!len) Fatal("Error linking program\n");
}

// Create Shader
int CreateShader(GLenum type, char* file) {
    //  Create the shader
    int shader = glCreateShader(type);
    //  Load source code from file
    char* source = ReadText(file);
    glShaderSource(shader, 1, (const char**)&source, NULL);
    free(source);
    //  Compile the shader
    fprintf(stderr, "Compile %s\n",file);
    glCompileShader(shader);
    //  Check for errors
    PrintShaderLog(shader, file);
    //  Return name
    return shader;
}

// Create Shader Program
int CreateShaderProg(char* VertFile, char* FragFile) {
    //  Create program
    int prog = glCreateProgram();
    //  Create and compile vertex shader
    int vert = CreateShader(GL_VERTEX_SHADER, VertFile);
    //  Create and compile fragment shader
    int frag = CreateShader(GL_FRAGMENT_SHADER, FragFile);
    //  Attach vertex shader
    glAttachShader(prog, vert);
    //  Attach fragment shader
    glAttachShader(prog, frag);
    //  Link program
    glLinkProgram(prog);
    //  Check for errors
    PrintProgramLog(prog);
    //  Return name
    return prog;
}
