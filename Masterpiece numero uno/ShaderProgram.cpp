
#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char *vertexShaderFile, const char *fragmentShaderFile) {
    
    // create the vertex shader
    vertexShader = loadShaderFromFile(vertexShaderFile, GL_VERTEX_SHADER);
    // create the fragment shader
    fragmentShader = loadShaderFromFile(fragmentShaderFile, GL_FRAGMENT_SHADER);
    
    // Create the final shader program from our vertex and fragment shaders
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);
    
    GLint linkSuccess;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkSuccess);
    if(linkSuccess == GL_FALSE) {
        printf("Error linking shader program!\n");
    }
    
    modelMatrixUniform = glGetUniformLocation(programID, "modelMatrix");
    projectionMatrixUniform = glGetUniformLocation(programID, "projectionMatrix");
    viewMatrixUniform = glGetUniformLocation(programID, "viewMatrix");
    
    positionAttribute = glGetAttribLocation(programID, "position");
    texCoordAttribute = glGetAttribLocation(programID, "texCoord");
    
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(programID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

GLuint ShaderProgram::loadShaderFromFile(const std::string &shaderFile, GLenum type) {
    //Open a file stream with the file name
    std::ifstream infile(shaderFile);
    
    if(infile.fail()) {
        std::cout << "Error opening shader file:" << shaderFile << std::endl;
    }
    
    //Create a string buffer and stream the file to it
    std::stringstream buffer;
    buffer << infile.rdbuf();
    
    // Load the shader from the contents of the file
    return loadShaderFromString(buffer.str(), type);
}

GLuint ShaderProgram::loadShaderFromString(const std::string &shaderContents, GLenum type) {
    
    
    // Create a shader of specified type
    GLuint shaderID = glCreateShader(type);
    
    // Get the pointer to the C string from the STL string
    const char *shaderString = shaderContents.c_str();
    GLint shaderStringLength = (GLint) shaderContents.size();
    
    // Set the shader source to the string and compile shader
    glShaderSource(shaderID, 1, &shaderString, &shaderStringLength);
    glCompileShader(shaderID);
    
    // Check if the shader compiled properly
    GLint compileSuccess;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileSuccess);
    
    // If the shader did not compile, print the error to stdout
    if (compileSuccess == GL_FALSE) {
        GLchar messages[512];
        glGetShaderInfoLog(shaderID, sizeof(messages), 0, &messages[0]);
        std::cout << messages << std::endl;
    }
    
    // return the shader id
    return shaderID;
}

void ShaderProgram::setViewMatrix(const Matrix &matrix) {
    glUseProgram(programID);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, matrix.ml);
}

void ShaderProgram::setModelMatrix(const Matrix &matrix) {
    glUseProgram(programID);
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, matrix.ml);
}

void ShaderProgram::setProjectionMatrix(const Matrix &matrix) {
    glUseProgram(programID);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, matrix.ml);    
}
