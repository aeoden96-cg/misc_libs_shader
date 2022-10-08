//
// Created by mateo on 13. 11. 2021..
//

#include "Shader.h"
#include <string>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;
Shader::Shader() = default;



void Shader::checkCompileErrors(unsigned int shader, const std::string& type)    {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "      ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return;

        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "      ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return;
        }
    }
}

GLuint Shader::create_compute_program() {
    std::string computeShaderPath = "ComputeShader.comp";

    std::string computeShaderCode;

    std::ifstream computeShaderFile;

    computeShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        computeShaderFile.open(computeShaderPath);
        std::stringstream computeShaderStream;
        computeShaderStream << computeShaderFile.rdbuf();
        computeShaderFile.close();
        computeShaderCode = computeShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "      ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* computeShaderCodeChar = computeShaderCode.c_str();

    GLuint ray_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(ray_shader, 1, &computeShaderCodeChar, NULL);
    glCompileShader(ray_shader);
    // check for compilation errors as per normal here

    GLuint ray_program = glCreateProgram();
    glAttachShader(ray_program, ray_shader);
    glLinkProgram(ray_program);

    return ray_program;

}


GLuint Shader::create_quad_program() {
    std::string vertexFile = "quad_shader.vert";
    std::string fragmentFile = "quad_shader.frag";

    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vShaderFile.open(vertexFile);
        fShaderFile.open(fragmentFile);

        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "      ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint program     = glCreateProgram();
    GLuint vert_shader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vert_shader, 1, &vShaderCode, NULL );
    glCompileShader( vert_shader );
    glAttachShader( program, vert_shader );
    GLuint frag_shader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( frag_shader, 1, &fShaderCode, NULL );
    glCompileShader( frag_shader );
    glAttachShader( program, frag_shader );
    glLinkProgram( program );
    return program;
}


unsigned int Shader::load_shaders(std::initializer_list<std::string> ll){

    std::vector<std::string> l;
    for(const auto& i: ll) l.push_back(i);

    if (ll.size() ==2){
        l.emplace_back("");
        l.emplace_back("");
        l.emplace_back("");
    }
    else if (ll.size() !=5) throw std::logic_error("not implemented for <5");



    std::map<int,std::string> shaders;
    std::vector<GLuint> shader_compiled_ids;
    int i = 0;

    int shader_codes[] = {
            GL_VERTEX_SHADER,
            GL_FRAGMENT_SHADER,
            GL_GEOMETRY_SHADER,
            GL_TESS_CONTROL_SHADER,
            GL_TESS_EVALUATION_SHADER

    };

    enum FileTypes_ { DEFAULT, AUTO };
    std::map<FileTypes_, std::string> pathType = {
            { DEFAULT, "DEFAULT" }, {AUTO, "AUTO"}
    };


    std::string defaultFileNames[] = {
            "vert.vert",
            "frag.frag",
            "geom.geom",
            "tesc.tesc",
            "tese.tese"
    };



    for(const auto& shaderPath : l) {

        if (shaderPath.empty()) {
            i++;
            continue;
        }

        if (pathType[DEFAULT] == shaderPath) {
            //pass default shader
            shaders[shader_codes[i]]=defaultFileNames[i];


        } else if (pathType[AUTO] == shaderPath) {
            //search for extension

        } else {
            if (shader_codes[i] == GL_TESS_CONTROL_SHADER) usingTess = true;
            //given explicit path name
            shaders[shader_codes[i]]=shaderPath;
        }
        i++;

    }
    std::cout << "Number of shaders: " << shaders.size()<<std::endl;
    for (const auto& [shader_id, shader_path] : shaders) {
        //std::cout<< "     " << shader_id << shader_path <<std::endl;

    }

    for (const auto& [shader_id, shader_path] : shaders) {


        std::cout << "      - [" << shader_id << " "  << shader_path << "]\n";
        std::string code;
        std::ifstream file;
        file.exceptions (std::ifstream::failbit | std::ifstream::badbit);


        try
        {
            std::stringstream stream;
            //std::cout << shader_path.c_str() <<std::endl;

            file.open(shader_path.c_str());
            stream << file.rdbuf();
            file.close();

            code   = stream.str();

            //std::cout << code << std::endl;

        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "      ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
            return -1;
        }
        //std::cout << "      Successful read"<< std::endl;
        //std::cout << "      Length: "<<code.length() <<std::endl;



        const char* shaderCode = code.c_str();


        GLint id = glCreateShader(shader_id);
        shader_compiled_ids.push_back(id);
        glShaderSource(id, 1, &shaderCode, nullptr);
        glCompileShader(id);
        checkCompileErrors(id, "SHADER " + shader_path );



    }
    std::cout << "Shaders comiled." << std::endl;
    // shader Program
    ID = glCreateProgram();
    for (auto k : shader_compiled_ids){
        glAttachShader(ID, k);
    }

    glLinkProgram(ID);

    checkCompileErrors(ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    for (auto k : shader_compiled_ids)
        glDeleteShader(k);

    shaders.clear();


    return ID;





}

unsigned int Shader::loadShaders(std::initializer_list<std::string> l) {
    return Shader::load_shaders(l);
}
