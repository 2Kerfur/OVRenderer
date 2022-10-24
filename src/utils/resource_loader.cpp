#include "resource_loader.h"
#include <stb_image.h>
#include <engine_config.h>

std::string GetCurrentDir()
{
    //std::filesystem requires C++17
    std::filesystem::path ExecDir = std::filesystem::current_path(); //get current executable path
    std::string path_string{ ExecDir.u8string() }; //convert to string
    return path_string;

}

std::string LoadShader(std::string shader_path)
{
    std::string finalPath = (GetCurrentDir() + SHADERS_PATH + shader_path); // get shader path
    std::string file_content;
    std::getline(std::ifstream(finalPath), file_content, '\0'); //load shader to string

    return file_content;
}

void LoadModel(std::string model_path)
{

}

unsigned char* LoadTexture(std::string texture_path,
    int* width, int* height, int* channels_in_file, bool filp_image)
{
    std::string finalPath = finalPath = (GetCurrentDir() + TEXTURES_PATH + texture_path); //get image path
   
    //Spesific for rendering api
    if (filp_image) {
        //stbi_set_flip_vertically_on_load(true); 
    }
    else {
       // stbi_set_flip_vertically_on_load(false);
    }

    //return stbi_load(finalPath.c_str(), width, height, channels_in_file, 0); //load image
    return 0;
}
