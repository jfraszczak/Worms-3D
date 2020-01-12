#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <cmath>
#include <cstdio>
#include <ctime>
using namespace std;

//Wszystkie potrzebne zmienne globalne
float speed_x=0;
float speed_y=0;
float aspectRatio=1;
int turn = -1;
int previous_turn = -1;
int combat_mode = 0;
int previous_combat_mode = 0;
bool even_number = true;
int start;
int stage = 0;
int winner = 0;
int chosen[] = {0, 0, 0 ,0};
int choosing = 0;
int who_is_choosing = -1;
int hit = 0;

ShaderProgram *sp;
ShaderProgram *sp_tex;

//Uchwyty na tekstury
GLuint tex0;
GLuint tex1;
GLuint tex2;
GLuint tex3;
GLuint tex4;
GLuint tex5;

//Klasa do pobierania wierzchołków modelu .obj
class Model{
    public:
        std::vector < float > vertices;
        std::vector < float > textures;
        std::vector < float > normals;

        std::vector < float > vertices3;
        std::vector < float > textures3;
        std::vector < float > normals3;

        std::vector < float > vertices4;
        std::vector < float > textures4;
        std::vector < float > normals4;

        std::vector < int > index_vert_troj;
        std::vector < int > index_tex_troj;
        std::vector < int > index_norm_troj;

        std::vector < int > index_vert_czworo;
        std::vector < int > index_tex_czworo;
        std::vector < int > index_norm_czworo;

        std::vector <string> split_line(string line)
        {
            std::vector < string > splitted;
            string pom = "";
            for(int i = 0; i < line.length(); i++)
            {
                if(line[i] == ' ')
                {
                    splitted.push_back(pom);
                    pom = "";
                }
                else
                {
                    pom += line[i];
                }
            }
            splitted.push_back(pom);
            return splitted;
        }

        int wyst(string line)
        {
            int licznik = 0;
            for(int i =0; i<line.length();i++)
            {
                if(line[i] == '/')
                {
                    licznik ++;
                }
            }
            return licznik;
        }

        string f_line(string line)
        {
            string newline = line.substr(2);
            for(int i = 0; i<newline.length(); i++)
            {
                if(newline[i] == ' ')
                {
                    newline[i] = '/';
                }
            }
            return newline;
        }

        int str2int(string str)
        {
            int integer = 0;
            for(int i = 0; i < str.size(); i++)
            {
                integer = integer * 10 + int(str[i] - '0');
            }
            return integer;
        }

        float str2f(string str)
        {
            float d, integer = 0, decimal = 0, k = 0.1;
            int sign, flag = 0;
            if(str[0] == '-'){sign = -1;}
            else{sign = 1;}
            for(int i = 0; i < str.length(); i++)
            {
                if(str[i] >= '0' && str[i] <= '9')
                {
                    if(flag == 0)
                    {
                        integer *= 10;
                        integer += int(str[i] - '0');
                    }
                    else
                    {
                        decimal += k * int(str[i] - '0');
                        k /= 10;
                    }
                }
                else if(str[i] == '.'){flag = 1;}
            }
            d = sign * (integer + decimal);
            return d;
        }

    void ObjLoader(string file) {
      string line, splitted;
      ifstream myfile;
      myfile.open(file.c_str());
      while ( getline (myfile,line) )
      {
        if(line[0] == 'v' && line[1] == ' ')
        {
            if(line[2] == ' ')
            {
                line.erase(1,1);
            }
            for(int i = 1; i < split_line(line).size(); i++)
            {
                splitted = split_line(line)[i];
                this -> vertices.push_back(str2f(splitted));
            }
        }
        else if(line[0] == 'v' && line[1] == 't')
        {
            for(int i = 1; i < split_line(line).size(); i++)
            {
                splitted = split_line(line)[i];
                this -> textures.push_back(str2f(splitted));
            }
        }
        else if(line[0] == 'v' && line[1] == 'n')
        {
            for(int i = 1; i < split_line(line).size(); i++)
            {
                splitted = split_line(line)[i];
                this -> normals.push_back(str2f(splitted));
            }
        }
        else if(line[0] == 'f')
        {
            string newline = f_line(line);
            int licznik = 0;
            string pom = "";
            int n = wyst(newline);
            for(int i =0; i<newline.length(); i++)
            {
                if(newline[i] == '/')
                {
                    if(licznik == 0)
                    {
                        if(n > 9)
                        {
                            this -> index_vert_czworo.push_back(str2int(pom) - 1);
                        }
                        else
                        {
                            this -> index_vert_troj.push_back(str2int(pom) - 1);
                        }
                    }
                    if(licznik == 1)
                    {
                        if(pom != ""){
                            if(n > 9)
                            {
                                this -> index_tex_czworo.push_back(str2int(pom) - 1);
                            }
                            else
                            {
                                this -> index_tex_troj.push_back(str2int(pom) - 1);
                            }
                        }
                    }
                    if(licznik == 2)
                    {
                        if(n > 9)
                        {
                            this -> index_norm_czworo.push_back(str2int(pom) - 1);
                        }
                        else
                        {
                            this -> index_norm_troj.push_back(str2int(pom) - 1);
                        }
                    }
                    if(licznik == 2)
                    {
                        licznik = 0;
                    }
                    else{
                        licznik ++;
                    }
                    pom = "";
                }
                else
                {
                    pom = pom + newline[i];
                }
            }
            if(n > 9)
            {
                this -> index_norm_czworo.push_back(str2int(pom) - 1);
            }
            else
            {
                this -> index_norm_troj.push_back(str2int(pom) - 1);
            }
        }
      }
      for(int i = 0; i < this -> index_vert_troj.size(); i++)
      {
          for(int j = 0; j < 3; j++)
          {
              this -> vertices3.push_back(this -> vertices[3*this -> index_vert_troj[i] + j]);
              this -> normals3.push_back(this -> normals[3*this -> index_norm_troj[i] + j]);
          }
          this -> vertices3.push_back(1.0f);
          this -> normals3.push_back(0.0f);
          if(textures.size() > 0)
          {
              for(int j = 0; j < 2; j++)
              {
                  this -> textures3.push_back(this -> textures[2*this -> index_tex_troj[i] + j]);
              }
          }
      }
      for(int i = 0; i < this -> index_vert_czworo.size(); i++)
      {
          for(int j = 0; j < 3; j++)
          {
              this -> vertices4.push_back(this -> vertices[3*this -> index_vert_czworo[i] + j]);
              this -> normals4.push_back(this -> normals[3*this -> index_norm_czworo[i] + j]);
          }
          this -> vertices4.push_back(1.0f);
          this -> normals4.push_back(0.0f);
          if(textures.size() > 0)
          {
              for(int j = 0; j < 2; j++)
              {
                  this -> textures4.push_back(this -> textures[2*this -> index_tex_czworo[i] + j]);
              }
          }
      }
      myfile.close();
    }
};

//Funkcja do załądowanai wysokości
std::vector<float>H;
void LoadHeights(){
    string line;
    ifstream myfile;
    myfile.open("heights.txt");
    int i = 0;
    while ( getline (myfile,line) ){
        H.push_back(atof(line.c_str()));
    }
    myfile.close();
}

//Klasa do generowania terenu - inicjalizuje wysokości terenu
class Terrain
{
    public:
        std::vector < float > vertices;
        std::vector < float > textures;
        std::vector < float > normals;
        std::vector<vector<float> > heights;
        float vertex_count;
        float SIZE;

        Terrain()
        {
            vertex_count = 1000;
            SIZE = 100;
            std::vector < float > verts(vertex_count * vertex_count * 3);
            std::vector < float > texts(vertex_count * vertex_count * 2);
            std::vector < float > norm(vertex_count * vertex_count * 3);
            std::vector < float > indices((vertex_count - 1) * (vertex_count - 1) * 6);
            vector< vector<float> > heights((vertex_count - 1) * (vertex_count - 1) * 2, vector<float>(3));
            int vertex_pointer = 0;
            LoadHeights();
            for(int i = 0; i < vertex_count; i++)
            {
                for(int j = 0; j < vertex_count; j++)
                {
                    verts[vertex_pointer * 3] = float(j) / (vertex_count - 1) * SIZE;
                    verts[vertex_pointer * 3 + 1] = H[i * vertex_count + j] * 8;
                    verts[vertex_pointer * 3 + 2] = float(i) / (vertex_count - 1) * SIZE;
                    norm[vertex_pointer * 3] = 0;
                    norm[vertex_pointer * 3 + 1] = 1;
                    norm[vertex_pointer * 3 + 2] = 0;
                    texts[vertex_pointer * 2] = float(j) / (vertex_count - 1);
                    texts[vertex_pointer * 2 + 1] = float(i) / (vertex_count - 1);
                    vertex_pointer++;
                }
            }

            int pointer = 0;
            for(int z = 0; z < vertex_count - 1; z++)
            {
                for(int x = 0; x < vertex_count - 1; x++)
                {
                    int top_left = (z * vertex_count) + x;
                    int top_right = top_left + 1;
                    int bottom_left = ((z + 1) * vertex_count) + x;
                    int bottom_right = bottom_left + 1;
                    indices[pointer++] = top_left;
                    indices[pointer++] = bottom_left;
                    indices[pointer++] = top_right;
                    indices[pointer++] = top_right;
                    indices[pointer++] = bottom_left;
                    indices[pointer++] = bottom_right;
                }
            }

            for(int i = 0; i < indices.size(); i++)
            {
                for(int j = 0; j < 3; j++)
                {
                    this -> vertices.push_back(verts[3 * indices[i] + j]);
                    this -> normals.push_back(norm[3 * indices[i] + j]);
                }
                //this->vertices.push_back(1.0f);
                //this->normals.push_back(0.0f);
                for(int j = 0; j < 2; j++)
                {
                    this -> textures.push_back(texts[2 * indices[i] + j]);
                }
            }
        }
        void DrawTerrain()
        {
            glm::mat4 M=glm::mat4(1.0f);
            M=glm::translate(M,glm::vec3(-SIZE / 2,0.0f,-SIZE / 2));
            glUniformMatrix4fv(sp_tex->u("M"),1,false,glm::value_ptr(M));

            glUniform1i(sp_tex->u("textureMap0"),0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,tex5);

            glEnableVertexAttribArray(sp_tex->a("vertex"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu vertex
            glVertexAttribPointer(sp_tex->a("vertex"),3,GL_FLOAT,false,0,&(this->vertices[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu vertex

            glEnableVertexAttribArray(sp_tex->a("normal"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu normal
            glVertexAttribPointer(sp_tex->a("normal"),3,GL_FLOAT,false,0,&(this->normals[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu normal

            glEnableVertexAttribArray(sp_tex->a("texCoord0"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu texCoord0
            glVertexAttribPointer(sp_tex->a("texCoord0"),2,GL_FLOAT,false,0,&(this->textures[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu texCoord0

            glDrawArrays(GL_TRIANGLES,0,this->vertices.size()/3); //Narysuj obiekt

            glDisableVertexAttribArray(sp_tex->a("vertex"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu vertex
            glDisableVertexAttribArray(sp_tex->a("normal"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu normal
            glDisableVertexAttribArray(sp_tex->a("texCoord0"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu texCoord0
        }
};

Terrain terrain;

//Funkja matematyczna służąca do obliczania wysokości punktu wewnątrz trójkąta
float BarycentricCoordinates(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x, float z)
{
    float det = (z2 - z3) * (x1 - x3) + (x3 - x2) * (z1 - z3);
    float l1 = ((z2 - z3) * (x - x3) + (x3 - x2) * (z - z3)) / det;
    float l2 = ((z3 - z1) * (x - x3) + (x1 - x3) * (z - z3)) / det;
    float l3 = 1.0f - l1 - l2;
    return l1 * y1 + l2 * y2 + l3 * y3;
}

//Klasa czołgu przechowuje jego współrzędne, pasek życia, kolor oraz umożliwia rysowanie paska życia i obliczanie współrzędnej y
class Tank
{
    public:
        float x;
        float y;
        float z;
        float angle;
        float hp;
        int type;
        int number;
    Tank()
    {
        x = 0;
        y = 0;
        z = 0;
        angle = 0;
        hp = 100;
    }
    void drawHP_Bar()
    {
        if(this -> hp < 0){this -> hp = 0;}
        float length = this -> hp / 100.0f * 2.0f;

        float verts1[] = {length -1.0f, 0.25f, 0.02f, length  -1.0f, -0.25f, 0.02f, - 1.0f, -0.25f, 0.02f,  - 1.0f, 0.25f, 0.02f};
        float verts2[] = {1.0f- length, 0.25f, -0.02f, 1.0f- length, -0.25f, -0.02f, 1.0f, -0.25f, -0.02f, 1.0f, 0.25f, -0.02f};
        float verts3[] = {-1.05f, 0.3f, 0.0f, -1.05f, -0.3f, 0.0f, 1.05f, -0.3f, 0.0f, 1.05f, 0.3f, 0.0f};

        glUniform4f(sp->u("color"),1,0,0,1);
        glEnableVertexAttribArray(sp->a("vertex"));
        glVertexAttribPointer(sp->a("vertex"),3,GL_FLOAT,false,0,verts1);
        glDrawArrays(GL_QUADS, 0, 4);
        glDisableVertexAttribArray(sp->a("vertex"));

        glUniform4f(sp->u("color"),1,0,0,1);
        glEnableVertexAttribArray(sp->a("vertex"));
        glVertexAttribPointer(sp->a("vertex"),3,GL_FLOAT,false,0,verts2);
        glDrawArrays(GL_QUADS, 0, 4);
        glDisableVertexAttribArray(sp->a("vertex"));

        glUniform4f(sp->u("color"),0.7,0.7,0.7,1);
        glEnableVertexAttribArray(sp->a("vertex"));
        glVertexAttribPointer(sp->a("vertex"),3,GL_FLOAT,false,0,verts3);
        glDrawArrays(GL_QUADS, 0, 4);
        glDisableVertexAttribArray(sp->a("vertex"));
    }

    void LoadType()
    {
        if(this -> type == 0) glUniform4f(sp->u("color"),1,0,1,1);
        if(this -> type == 1) glUniform4f(sp->u("color"),0,0.3,1,1);
        if(this -> type == 2) glUniform4f(sp->u("color"),1.0,0.5,0,1);
        if(this -> type == 3) glUniform4f(sp->u("color"),1,1,0,1);
    }

    void LoadTexture(){
        glUniform1i(sp_tex->u("textureMap0"),0);
        glActiveTexture(GL_TEXTURE0);
        if(this -> type == 0) glBindTexture(GL_TEXTURE_2D,tex4);
        if(this -> type == 1) glBindTexture(GL_TEXTURE_2D,tex1);
        if(this -> type == 2) glBindTexture(GL_TEXTURE_2D,tex2);
        if(this -> type == 3) glBindTexture(GL_TEXTURE_2D,tex3);
    }

    void CalculateHeight()
    {
        float terrainX = this -> x + terrain.SIZE / 2;
        float terrainZ = this -> z + terrain.SIZE / 2;
        float gridSquareSize = terrain.SIZE / (terrain.vertex_count - 1);
        int gridX = int(floor(terrainX / gridSquareSize));
        int gridZ = int(floor(terrainZ / gridSquareSize));
        float gridCoordX = (terrainX - gridSquareSize * gridX) / gridSquareSize;
        float gridCoordZ = (terrainZ - gridSquareSize * gridZ) / gridSquareSize;
        if(gridCoordX >= 1 - gridCoordZ)
        {
            float y1, y2, y3;
            y1 = terrain.vertices[(gridX * (terrain.vertex_count - 1) + gridZ) * 6 * 3 + 0 * 3 + 1];
            y2 = terrain.vertices[(gridX * (terrain.vertex_count - 1) + gridZ) * 6 * 3 + 1 * 3 + 1];
            y3 = terrain.vertices[(gridX * (terrain.vertex_count - 1) + gridZ) * 6 * 3 + 2 * 3 + 1];
            this -> y = BarycentricCoordinates(0, y1, 0, 0, y2, 1, 1, y3, 0, gridCoordX, gridCoordZ) + 0.3;
        }
        else
        {
            float y1, y2, y3;
            y1 = terrain.vertices[(gridX * (terrain.vertex_count - 1) + gridZ) * 6 * 3 + 9 + 0 * 3 + 1];
            y2 = terrain.vertices[(gridX * (terrain.vertex_count - 1) + gridZ) * 6 * 3 + 9 + 1 * 3 + 1];
            y3 = terrain.vertices[(gridX * (terrain.vertex_count - 1) + gridZ) * 6 * 3 + 9 + 2 * 3 + 1];
            this -> y = BarycentricCoordinates(1, y1, 0, 0, y2, 1, 1, y3, 1, gridCoordX, gridCoordZ) + 0.3;
        }

    }
} ;

//Klasa do rysowania strzałki do wystrzału, przechowuje kąt oraz siłę wystrzału
class Arrow
{
    public:
        float angle;
        float power;

        Arrow()
        {
            angle = 0;
            power = 100;
        }

        void drawArrow()
        {
            float verts1[] = {-1.0f, 0.25f, 0.0f, -1.0f, -0.25f, 0.0f, this->power / 100 * 2.0f - 0.9f, -0.25f, 0.0f, this->power / 100 * 2.0 - 0.9f, 0.25f, 0.0f};
            float verts2[] = {this->power / 100 * 2.0 - 0.9f, 0.5f, 0.0f, this->power / 100 * 2.0 - 0.9f + 0.7f, 0.0f, 0.0f, this->power / 100 * 2.0 - 0.9f, -0.5f, 0.0f};

            glUniform4f(sp->u("color"),1.0,1.0,0.1,1);

            glEnableVertexAttribArray(sp->a("vertex"));
            glVertexAttribPointer(sp->a("vertex"),3,GL_FLOAT,false,0,verts1);
            glDrawArrays(GL_QUADS, 0, 4);
            glDisableVertexAttribArray(sp->a("vertex"));

            glEnableVertexAttribArray(sp->a("vertex"));
            glVertexAttribPointer(sp->a("vertex"),3,GL_FLOAT,false,0,verts2);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glDisableVertexAttribArray(sp->a("vertex"));
        }
};

//Deklaracja obiektów globalnych
Model model1;
Model model2;
Model model3;
Model trophy;
Tank tank1;
Tank tank2;
Arrow arrow;
Tank tank3;

//Klasa służąca do generowania eksplozji - system cząstek
class Particle
{
    public:
        float x;
        float y;
        float z;
        float speed_x;
        float speed_y;
        float speed_z;
        glm::mat4 M;
        float color;
        int time_to_live;

        Particle()
        {
            this -> x = 0;
            this -> y = 0;
            this -> z = 0;
            this -> speed_x = 0;
            this -> speed_y = 0;
            this -> speed_x = (rand() % 1000 - 500) * 0.0005;
            this -> speed_y = (rand() % 1000 - 500) * 0.0005;
            this -> speed_z = (rand() % 1000 - 500) * 0.0005;
            this -> time_to_live = 20;
        }

        void drawParticle(float rocket_x, float rocket_z)
        {
            if(this -> time_to_live >= 0)
            {
                this -> M=glm::mat4(1.0f);
                if(hit == 1) M=glm::translate(M,glm::vec3(rocket_x, tank1.y + 2.5f, rocket_z));
                else if(hit == 2) M=glm::translate(M,glm::vec3(rocket_x, tank2.y + 2.5f, rocket_z));

                M=glm::translate(M,glm::vec3(this ->x, this ->y, this ->z));
                glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));
                glUniform4f(sp->u("color"),1,color / 255,0,1);
                float verts[] = {-0.1f, -0.1f, 0.0f, 0.1f, -0.1f, 0.0f, 0.1f, 0.1f, 0.0f, -0.1f, 0.1f, 0.0f};
                glEnableVertexAttribArray(sp->a("vertex"));
                glVertexAttribPointer(sp->a("vertex"),3,GL_FLOAT,false,0,verts);
                glDrawArrays(GL_QUADS, 0, 4);
                glDisableVertexAttribArray(sp->a("vertex"));
            }
            this -> time_to_live--;
        }

        void update()
        {
            if(((sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2))) >= 0.5 + rand() % 10 * 0.1)){
                this -> x = 0;
                this -> y = 0;
                this -> z = 0;
                color = 255;
            }
            else
            {
                this -> x += speed_x * glfwGetTime();
                this -> y += speed_y * glfwGetTime();
                this -> z += speed_z * glfwGetTime();
                this -> color -= 10;
            }
        }

};

//Klasa służąca do generowania konfetti dla zwycięzcy - system cząstek
class Konfetti
{
    public:
        float x;
        float z;
        float y;
        float speed;
        float color;
        glm::mat4 M;

        Konfetti()
        {
            this -> x = rand() % 1000 * 0.02 - 10;
            this -> z = rand() % 1000 * 0.001;
            this -> y = 8;
            this -> speed = rand() % 1000 * 0.003 + 2;
            this -> color = rand() % 1000 * 0.001;
        }

        void update()
        {
            if(this -> y <= 0) this -> y = 8;
            else this -> y -=  this -> speed * glfwGetTime();
            this -> color = rand() % 1000 * 0.001;
        }

        void draw()
        {
            this -> M=glm::mat4(1.0f);
            M=glm::translate(M,glm::vec3(this -> x, this -> y, this -> z));
            glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));
            glUniform4f(sp->u("color"),1,color,1,1);
            float verts[] = {-0.03f, -0.03f, 0.0f, 0.03f, -0.03f, 0.0f, 0.03f, 0.03f, 0.0f, -0.031f, 0.03f, 0.0f};
            glEnableVertexAttribArray(sp->a("vertex"));
            glVertexAttribPointer(sp->a("vertex"),3,GL_FLOAT,false,0,verts);
            glDrawArrays(GL_QUADS, 0, 4);
            glDisableVertexAttribArray(sp->a("vertex"));
        }
};

//Klasa rysująca strzałkę wiatru oraz losująca jego prędkość i kąt
class Wind
{
    public:
        float speed;
        float angle;
        glm::mat4 M;
        float red, green;

        Wind()
        {
            this -> speed = float(rand() % 6);
            this -> angle = float(rand() % 8) * 45.0 / 180.0 * PI;
            if(this->speed >= 3)
            {
                red = 1.0;
                green = 1.0 - ((this->speed - 3.0) / 2.0);
            }
            else
            {
                red = this->speed / 2.0;
                green = 1.0;
            }
        }

        void randomise()
        {
            this -> speed = float(rand() % 6);
            this -> angle = float(rand() % 8) * 45.0 / 180.0 * PI;
            if(this->speed >= 3)
            {
                red = 1.0;
                green = 1.0 - ((this->speed - 3.0) / 2.0);
            }
            else
            {
                red = this->speed / 2.0;
                green = 1.0;
            }
        }

        void draw_arrow()
        {
            this -> M=glm::mat4(1.0f);
            if(turn == -1) M=glm::translate(M,glm::vec3(tank1.x + 1.0, tank1.y + 15.0, tank1.z + 15.0));
            else if(turn == 1) M=glm::translate(M,glm::vec3(tank2.x - 1.0, tank2.y + 15.0, tank2.z - 15.0));
            M=glm::rotate(M, PI / 2,glm::vec3(0.0f,0.0f,1.0f));
            M=glm::rotate(M, this->angle,glm::vec3(0.0f,0.0f,1.0f));
            M=glm::scale(M,glm::vec3(1.5f,1.5f,1.0f));

            glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));

            float verts1[] = {-1.0f, 0.1f, 0.0f, -1.0f, -0.1f, 0.0f, 1.0f - 0.9f, -0.1f, 0.0f, 1.0 - 0.9f, 0.1f, 0.0f};
            float verts2[] = {1.0 - 0.9f, 0.3f, 0.0f, 1.0 - 0.9f + 0.7f, 0.0f, 0.0f, 1.0 - 0.9f, -0.3f, 0.0f};

            glUniform4f(sp->u("color"),red,green,0.0,1);

            glEnableVertexAttribArray(sp->a("vertex"));
            glVertexAttribPointer(sp->a("vertex"),3,GL_FLOAT,false,0,verts1);
            glDrawArrays(GL_QUADS, 0, 4);
            glDisableVertexAttribArray(sp->a("vertex"));

            glEnableVertexAttribArray(sp->a("vertex"));
            glVertexAttribPointer(sp->a("vertex"),3,GL_FLOAT,false,0,verts2);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glDisableVertexAttribArray(sp->a("vertex"));
        }
};

Wind wind;

//Klasa do rysowania rakiety i aktualizacji jej współrzędnych oraz kąta
class Rocket
{
    public:
        float x;
        float y;
        float z;
        float angle_vertical;
        float angle_horizontal;
        float speed;
        float speed_x;
        float speed_y;
        float speed_z;

        void initialize_speeds()
        {
            float pom;
            pom = cos(this -> angle_vertical) * speed;
            this -> speed_y = sin(this -> angle_vertical) * speed;
            this -> speed_x = cos(- this -> angle_horizontal) * pom;
            this -> speed_z = sin(this -> angle_horizontal) * pom;
        }

        void update_angle()
        {
            this->speed_x += cos(wind.angle) * wind.speed * 0.02;
            this->speed_z += sin(wind.angle) * wind.speed * 0.02;
            this -> angle_vertical = atan(speed_y / sqrt(pow(speed_x, 2) + pow(speed_z, 2)));
            this -> angle_horizontal = atan(speed_z / speed_x);
        }

};

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

//Funkcja wczytująca teksturę
GLuint readTexture(const char* filename) {
  GLuint tex;
  glActiveTexture(GL_TEXTURE0);

  //Wczytanie do pamięci komputera
  std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
  unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
  //Wczytaj obrazek
  unsigned error = lodepng::decode(image, width, height, filename);

  //Import do pamięci karty graficznej
  glGenTextures(1,&tex); //Zainicjuj jeden uchwyt
  glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
  //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
  glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return tex;
}

//Deklaracja globalnych obiektów
Particle explosion[10000];
Konfetti konfetti[100];
Rocket rocket;

//Klasa przechowująca współrzędne gdzie nastąpiło uderzenie rakiety i zadająca odpowiednie obrażenia czołgom
class DealDamage
{
    public:
        float speed;
        float x;
        float y;
        float z;

    int collision1()
    {
        int damage = 0;
        if(this -> x >= -2 + tank1.x && this -> x <= 2 + tank1.x && this -> z >= -2 + tank1.z && this -> z <= 2 + tank1.z){damage = 20; hit = 1;}
        if(this -> x >= -1 + tank1.x && this -> x <= 1 + tank1.x && this -> z >= -1 + tank1.z && this -> z <= 1 + tank1.z){damage = 30; hit = 1;}
        if(this -> x >= -0.5 + tank1.x && this -> x <= 0.5 + tank1.x && this -> z >= -0.5 + tank1.z && this -> z <= 0.5 + tank1.z){damage = 40; hit = 1;}
        if(this -> x >= -0.3 + tank1.x && this -> x <= 0.3 + tank1.x && this -> z >= -0.3 + tank1.z && this -> z <= 0.3 + tank1.z){damage = 50; hit = 1;}
        return damage;
    }

    int collision2()
    {
        int damage = 0;
        if(this -> x >= -2 + tank2.x && this -> x <= 2 + tank2.x && this -> z >= -2 + tank2.z && this -> z <= 2 + tank2.z){damage = 20; hit = 2;}
        if(this -> x >= -1 + tank2.x && this -> x <= 1 + tank2.x && this -> z >= -1 + tank2.z && this -> z <= 1 + tank2.z){damage = 30; hit = 2;}
        if(this -> x >= -0.5 + tank2.x && this -> x <= 0.5 + tank2.x && this -> z >= -0.5 + tank2.z && this -> z <= 0.5 + tank2.z){damage = 40; hit = 2;}
        if(this -> x >= -0.3 + tank2.x && this -> x <= 0.3 + tank2.x && this -> z >= -0.3 + tank2.z && this -> z <= 0.3 + tank2.z){damage = 50; hit = 2;}
        return damage;
    }
};

DealDamage damage;

//Obsługa klawiszy
void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT)
        {
            if(stage == 1) speed_x=-PI;
            else if(stage == 0)
            {
                choosing--;
                if(choosing == -1){choosing = 3;}
                if(chosen[choosing] == 1)
                {
                    choosing--;
                    if(choosing == -1){choosing = 3;}
                }
            }
        }
        if (key==GLFW_KEY_RIGHT)
        {
            if(stage == 1) speed_x=PI;
            else if(stage == 0)
            {
                choosing = (choosing + 1) % 4;
                if(chosen[choosing] == 1){choosing = (choosing + 1) % 4;}
            }
        }
        if (key==GLFW_KEY_UP) speed_y=PI;
        if (key==GLFW_KEY_DOWN) speed_y=-PI;
        if (key==GLFW_KEY_K)
        {
            if(turn == 2){turn = previous_turn;}
            else
            {
                previous_turn = turn;
                turn = 2;
            }
        }
        if (key==GLFW_KEY_SPACE)
        {
            if(combat_mode < 3 && stage == 1)
            {
                previous_combat_mode = combat_mode;
                combat_mode += 1;
                even_number = true;
                if(combat_mode == 1){arrow.power = 100;}
            }
            else if(stage == 0)
            {
                chosen[choosing] = 1;
                if(who_is_choosing == -1)
                {
                    tank1.type = choosing;
                    who_is_choosing = 1;
                }
                else
                {
                    tank2.type = choosing;
                    stage = 1;
                    who_is_choosing = -1;
                }
                choosing = (choosing + 1) % 4;
            }
        }
        if (key==GLFW_KEY_ENTER && stage == 2)
        {
            winner = 0;
            stage = 0;
            tank1.hp = 100;
            tank2.hp = 100;
            tank1.x = -10;
            tank2.x = 10;
            tank1.z = 0;
            tank2.z = 0;
            tank1.angle = 0;
            tank2.angle = 0;
            choosing = 0;
            for(int i = 0; i < 4; i++) chosen[i] = 0;
        }
        if (key==GLFW_KEY_ESCAPE && stage == 1)
        {
            winner = -turn;
            stage = 2;
        }
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;
    }
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);

	sp=new ShaderProgram("vertex1.glsl",NULL,"fragment1.glsl");
	sp_tex=new ShaderProgram("vertex2.glsl",NULL,"fragment2.glsl");

    tex0=readTexture("sky.png");
    tex1=readTexture("bricks.png");
    tex2=readTexture("red.png");
    tex3=readTexture("metal.png");
    tex4=readTexture("gold1.png");
    tex5=readTexture("ground1.png");

    model1.ObjLoader("Czolk.obj");
    model2.ObjLoader("Czolk.obj");
    model3.ObjLoader("aim.obj");
    trophy.ObjLoader("trophy.obj");

    tank1.x = -10;
	tank2.x = 10;
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
    glDeleteTextures(1,&tex0);
    glDeleteTextures(1,&tex1);
    glDeleteTextures(1,&tex2);
    glDeleteTextures(1,&tex3);
    glDeleteTextures(1,&tex4);
    glDeleteTextures(1,&tex5);
    delete sp;
    delete sp_tex;
}

//Rysowanie pojedyńczego obiektu
void drawObject(ShaderProgram *sp,std::vector<float> vertices3, std::vector<float> normals3, std::vector<float> texCoords3, std::vector<float> vertices4, std::vector<float> normals4, std::vector<float> texCoords4) {
    glEnableVertexAttribArray(sp->a("vertex"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu vertex
    glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,&(vertices3[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu vertex

    glEnableVertexAttribArray(sp->a("normal"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu normal
    glVertexAttribPointer(sp->a("normal"),4,GL_FLOAT,false,0,&(normals3[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu normal

    glEnableVertexAttribArray(sp->a("texCoord0"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu texCoord0
    glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,&(texCoords3[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu texCoord0

    glDrawArrays(GL_TRIANGLES,0,vertices3.size()/4); //Narysuj obiekt

    glDisableVertexAttribArray(sp->a("vertex"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu vertex
    glDisableVertexAttribArray(sp->a("normal"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu normal
    glDisableVertexAttribArray(sp->a("texCoord0"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu texCoord0

    glEnableVertexAttribArray(sp->a("vertex"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu vertex
    glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,&(vertices4[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu vertex

    glEnableVertexAttribArray(sp->a("normal"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu normal
    glVertexAttribPointer(sp->a("normal"),4,GL_FLOAT,false,0,&(normals4[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu normal

    glEnableVertexAttribArray(sp->a("texCoord0"));  //WĹ‚Ä…cz przesyĹ‚anie danych do atrybutu texCoord0
    glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,&(texCoords4[0])); //WskaĹĽ tablicÄ™ z danymi dla atrybutu texCoord0

    glDrawArrays(GL_QUADS,0,vertices4.size()/4); //Narysuj obiekt

    glDisableVertexAttribArray(sp->a("vertex"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu vertex
    glDisableVertexAttribArray(sp->a("normal"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu normal
    glDisableVertexAttribArray(sp->a("texCoord0"));  //WyĹ‚Ä…cz przesyĹ‚anie danych do atrybutu texCoord0
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float rocket_x, float rocket_z) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
    glClearColor(0,0.7,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 V;
    //Kamera numer 1
	if(turn == - 1)
    {
        V=glm::lookAt(
        glm::vec3(-20+tank1.x, tank1.y + 15, tank1.z),
        glm::vec3(tank1.x + 20.0,tank1.y,tank1.z),
        glm::vec3(0.0f,3.0f,0.0f)); //Wylicz macierz widoku
    }
    //Kamera numer 2
    if(turn == 1)
    {
        V=glm::lookAt(
        glm::vec3(20+tank2.x, tank2.y + 15, tank2.z),
        glm::vec3(tank2.x - 20,tank2.y,tank2.z),
        glm::vec3(0.0f,3.0f,0.0f)); //Wylicz macierz widoku
    }
    //Kamera boczna
    if(turn == 2)
    {

        V=glm::lookAt(
        glm::vec3((tank1.x + tank2.x) / 2, 20, 30 + (tank1.z + tank2.z) / 2),
        glm::vec3((tank1.x + tank2.x) / 2,0,(tank1.z + tank2.z) / 2),
        glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz widoku
    }

    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania
    glm::mat4 M1=glm::mat4(1.0f);
    glm::mat4 M2=glm::mat4(1.0f);
    glm::mat4 M3=glm::mat4(1.0f);
    glm::mat4 M4=glm::mat4(1.0f);
    glm::mat4 M5=glm::mat4(1.0f);
    glm::mat4 M6=glm::mat4(1.0f);
    glm::mat4 M7=glm::mat4(1.0f);



    //Aktywacja programu cieniującego
    sp_tex->use();

    //Przeslij parametry programu cieniującego do karty graficznej
    glUniformMatrix4fv(sp_tex->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp_tex->u("V"),1,false,glm::value_ptr(V));

    glUniform4f(sp_tex->u("lp"),0, 10, -100,1); //Współrzędne źródła światła
    glUniform4f(sp_tex->u("lp2"),0, 10, 100,1); //Współrzędne źródła światła


    //Podłoże
    terrain.DrawTerrain();

    //Czołg numer 1
    M1=glm::translate(M1,glm::vec3(tank1.x,tank1.y,tank1.z));
    M1=glm::rotate(M1,- PI / 2,glm::vec3(1.0f,0.0f,0.0f));
    M1=glm::rotate(M1,-tank1.angle + PI,glm::vec3(0.0f,0.0f,1.0f)); //Wylicz macierz modelu

    glUniformMatrix4fv(sp_tex->u("M"),1,false,glm::value_ptr(M1));
    tank1.LoadTexture();
    drawObject(sp_tex, model1.vertices3, model1.normals3, model1.textures3, model1.vertices4, model1.normals4, model1.textures4);

    //Aktywacja programu cieniującego
    sp->use();

    //Przeslij parametry programu cieniującego do karty graficznej
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));

    //HP Bar numer 1
    M2=glm::translate(M2,glm::vec3(-1.0f + tank1.x, tank1.y + 4.0f, tank1.z));
    M2=glm::rotate(M2, PI / 2,glm::vec3(0.0f,1.0f,0.0f));
    if(turn == 2){M2=glm::rotate(M2, PI / 2,glm::vec3(0.0f,1.0f,0.0f));}
    if (turn == -1 || turn == 2){M2=glm::rotate(M2, PI / 9,glm::vec3(1.0f,0.0f,0.0f));}
    else {M2=glm::rotate(M2, -PI / 9,glm::vec3(1.0f,0.0f,0.0f));}
    if(turn  == 2){M2=glm::scale(M2,glm::vec3(1.7f,1.05f,1.0f));}
    else{M2=glm::scale(M2,glm::vec3(1.3f,0.8f,1.0f));}
	glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M2));
	tank1.drawHP_Bar();

	//Arrow numer 1
	if(combat_mode > 0 && (turn == -1 || (turn == 2 && previous_turn == -1)))
    {
        M3=glm::translate(M3,glm::vec3(1.0f + tank1.x, tank1.y + 7.0f, tank1.z));
        M3=glm::rotate(M3, -tank1.angle,glm::vec3(0.0f,1.0f,0.0f));
        M3=glm::rotate(M3, arrow.angle,glm::vec3(0.0f,0.0f,1.0f));
        if(turn != 2){M3=glm::rotate(M3, PI / 2,glm::vec3(1.0f,0.0f,0.0f));}
        M3=glm::scale(M3,glm::vec3(2.0f,1.0f,1.0f));
        glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M3));
        arrow.drawArrow();
    }

    //Rocket numer 1
    if(combat_mode == 3 && (turn == -1 || (turn == 2 && previous_turn == -1)))
    {
        M7=glm::translate(M7,glm::vec3(rocket.x, rocket.y, rocket.z));
        M7=glm::rotate(M7, -PI / 2 ,glm::vec3(0.0f,1.0f,0.0f));
        M7=glm::rotate(M7, rocket.angle_vertical ,glm::vec3(1.0f,0.0f,0.0f));
        M7=glm::rotate(M7, -rocket.angle_horizontal ,glm::vec3(0.0f,1.0f,0.0f));
        M7=glm::scale(M7,glm::vec3(0.005f,0.005f,0.005f));
        sp_tex->use();
        glUniformMatrix4fv(sp_tex->u("P"),1,false,glm::value_ptr(P));
        glUniformMatrix4fv(sp_tex->u("V"),1,false,glm::value_ptr(V));

        glUniform4f(sp_tex->u("lp"),-2.5,2,3,1); //Współrzędne źródła światła
        glUniform4f(sp_tex->u("lp2"),2.5,2,3,1); //Współrzędne źródła światła

        glUniform1i(sp_tex->u("textureMap0"),0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,tex1);
        glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M7));
        drawObject(sp_tex, model3.vertices3, model3.normals3, model3.textures3, model3.vertices4, model3.normals4, model3.textures4);
    }

	//Czołg numer 2
    M4=glm::translate(M4,glm::vec3(tank2.x,tank2.y,tank2.z));
    M4=glm::rotate(M4,-PI / 2,glm::vec3(1.0f,0.0f,0.0f));
    M4=glm::rotate(M4,tank2.angle,glm::vec3(0.0f,0.0f,1.0f)); //Wylicz macierz modelu

    //Aktywacja programu cieniującego
    sp_tex->use();

    //Przeslij parametry programu cieniującego do karty graficznej
    glUniformMatrix4fv(sp_tex->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp_tex->u("V"),1,false,glm::value_ptr(V));

    glUniformMatrix4fv(sp_tex->u("M"),1,false,glm::value_ptr(M4));
    tank2.LoadTexture();
    drawObject(sp_tex, model2.vertices3, model2.normals3, model2.textures3, model2.vertices4, model2.normals4, model2.textures4);

    //Aktywacja programu cieniującego
    sp->use();

    //Przeslij parametry programu cieniującego do karty graficznej
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));

    //HP Bar numer 2
    M5=glm::translate(M5,glm::vec3(1.0f + tank2.x,tank1.y + 4.0f,tank2.z));
    M5=glm::rotate(M5, PI / 2,glm::vec3(0.0f,1.0f,0.0f));
    if(turn == 2){M5=glm::rotate(M5, PI / 2,glm::vec3(0.0f,1.0f,0.0f));}
    if (turn == 1){M5=glm::rotate(M5, -PI / 9,glm::vec3(1.0f,0.0f,0.0f));}
    else {M5=glm::rotate(M5, PI / 9,glm::vec3(1.0f,0.0f,0.0f));}
    if(turn == 2){M5=glm::scale(M5,glm::vec3(1.7f,1.05f,1.0f));}
    else{M5=glm::scale(M5,glm::vec3(1.3f,0.8f,1.0f));}
	glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M5));
	tank2.drawHP_Bar();

    //Arrow numer 2
	if(combat_mode > 0 && (turn == 1 || (turn == 2 && previous_turn == 1)))
    {
        M6=glm::translate(M6,glm::vec3(-1.0f + tank2.x, tank1.y + 7.0f, tank2.z));
        M6=glm::rotate(M6, tank2.angle,glm::vec3(0.0f,1.0f,0.0f));
        M6=glm::rotate(M6, PI ,glm::vec3(0.0f,1.0f,0.0f));
        M6=glm::rotate(M6, arrow.angle,glm::vec3(0.0f,0.0f,1.0f));
        if(turn != 2){M6=glm::rotate(M6, PI / 2,glm::vec3(1.0f,0.0f,0.0f));}
        M6=glm::scale(M6,glm::vec3(2.0f,1.0f,1.0f));
        glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M6));
        arrow.drawArrow();
    }

    //Rocket numer 2
    if(combat_mode == 3 && (turn == 1 || (turn == 2 && previous_turn == 1)))
    {
        M7=glm::translate(M7,glm::vec3(rocket.x, rocket.y, rocket.z));
        M7=glm::rotate(M7, PI / 2 ,glm::vec3(0.0f,1.0f,0.0f));
        M7=glm::rotate(M7, rocket.angle_vertical ,glm::vec3(1.0f,0.0f,0.0f));
        M7=glm::rotate(M7, rocket.angle_horizontal ,glm::vec3(0.0f,1.0f,0.0f));
        M7=glm::scale(M7,glm::vec3(0.005f,0.005f,0.005f));
        sp_tex->use();
        glUniformMatrix4fv(sp_tex->u("P"),1,false,glm::value_ptr(P));
        glUniformMatrix4fv(sp_tex->u("V"),1,false,glm::value_ptr(V));

        glUniform1i(sp_tex->u("textureMap0"),0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,tex1);
        glUniformMatrix4fv(sp_tex->u("M"),1,false,glm::value_ptr(M7));
        drawObject(sp_tex, model3.vertices3, model3.normals3, model3.textures3, model3.vertices4, model3.normals4, model3.textures4);
    }

    //Eksplozja
    if(hit != 0)
    {
        for(int i = 0; i < 10000; i++)
        {
            explosion[i].drawParticle(rocket_x, rocket_z);
        }
    }

    //Wiatr
    if(turn == -1 || turn == 1) wind.draw_arrow();

    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}

//Scena rysowana po wygraniu gry
void drawVictoryScene(GLFWwindow* window, float angle) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,1);
    glm::mat4 V;
    V=glm::lookAt(
    glm::vec3(0, 5, 10),
    glm::vec3(0,2,0),
    glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz widoku
    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania
    glm::mat4 M1=glm::mat4(1.0f);
    glm::mat4 M2=glm::mat4(1.0f);
    glm::mat4 M3=glm::mat4(1.0f);


    //Aktywacja programu cieniującego
    sp_tex->use();

    //Przeslij parametry programu cieniującego do karty graficznej
    glUniformMatrix4fv(sp_tex->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp_tex->u("V"),1,false,glm::value_ptr(V));

    M1=glm::rotate(M1,-PI / 2,glm::vec3(1.0f,0.0f,0.0f));
    M1=glm::rotate(M1, angle,glm::vec3(0.0f,0.0f,1.0f));
    M1=glm::scale(M1,glm::vec3(0.9f,0.9f,0.9f));
    glUniformMatrix4fv(sp_tex->u("M"),1,false,glm::value_ptr(M1));

    if(winner == -1)
    {
        tank1.LoadTexture();
        drawObject(sp_tex, model1.vertices3, model1.normals3, model1.textures3, model1.vertices4, model1.normals4, model1.textures4);
    }
    else if(winner == 1)
    {
        tank2.LoadTexture();
        drawObject(sp_tex, model2.vertices3, model2.normals3, model2.textures3, model2.vertices4, model2.normals4, model2.textures4);
    }

    glUniform4f(sp_tex->u("lp"),1, 2, 6,1); //Współrzędne źródła światła
    glUniform1i(sp_tex->u("textureMap0"),0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex4);
    M2=glm::translate(M2,glm::vec3(0.0f, 3.0f, 0.0f));
    M2=glm::scale(M2,glm::vec3(0.4f,0.4f,0.4f));
    glUniformMatrix4fv(sp_tex->u("M"),1,false,glm::value_ptr(M2));
    drawObject(sp_tex, trophy.vertices3, trophy.normals3, trophy.textures3, trophy.vertices4, trophy.normals4, trophy.textures4);



    //Aktywacja programu cieniującego
    sp->use();

    //Przeslij parametry programu cieniującego do karty graficznej
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));

    //Konfetti
    for(int i = 0; i < 100; i++)
    {
        konfetti[i].draw();
    }

    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}

//Scena rysowana w trakcie wyboru czołgu
void drawChoosingScene(GLFWwindow* window, float angle){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,1);
    glm::mat4 V;
    V=glm::lookAt(
    glm::vec3(0, 4, 15),
    glm::vec3(0,0,0),
    glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz widoku
    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania
    glm::mat4 M1=glm::mat4(1.0f);
    glm::mat4 M2=glm::mat4(1.0f);
    glm::mat4 M3=glm::mat4(1.0f);
    glm::mat4 M4=glm::mat4(1.0f);

    M1=glm::translate(M1,glm::vec3(-7.5f, 0.0f, 0.0f));
    M2=glm::translate(M2,glm::vec3(-2.5f, 0.0f, 0.0f));
    M3=glm::translate(M3,glm::vec3(2.5f, 0.0f, 0.0f));
    M4=glm::translate(M4,glm::vec3(7.5f, 0.0f, 0.0f));
    if(choosing == 0 or chosen[0] == 1) M1=glm::translate(M1,glm::vec3(0.0f, 0.0f, 3.0f));
    if(choosing == 1 or chosen[1] == 1) M2=glm::translate(M2,glm::vec3(0.0f, 0.0f, 3.0f));
    if(choosing == 2 or chosen[2] == 1) M3=glm::translate(M3,glm::vec3(0.0f, 0.0f, 3.0f));
    if(choosing == 3 or chosen[3] == 1) M4=glm::translate(M4,glm::vec3(0.0f, 0.0f, 3.0f));

    M1=glm::rotate(M1,-PI / 2,glm::vec3(1.0f,0.0f,0.0f));
    M1=glm::rotate(M1, angle,glm::vec3(0.0f,0.0f,1.0f));

    M2=glm::rotate(M2,-PI / 2,glm::vec3(1.0f,0.0f,0.0f));
    M2=glm::rotate(M2, angle,glm::vec3(0.0f,0.0f,1.0f));

    M3=glm::rotate(M3,-PI / 2,glm::vec3(1.0f,0.0f,0.0f));
    M3=glm::rotate(M3, angle,glm::vec3(0.0f,0.0f,1.0f));

    M4=glm::rotate(M4,-PI / 2,glm::vec3(1.0f,0.0f,0.0f));
    M4=glm::rotate(M4, angle,glm::vec3(0.0f,0.0f,1.0f));

    sp_tex->use();

    glUniformMatrix4fv(sp_tex->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp_tex->u("V"),1,false,glm::value_ptr(V));

    glUniform4f(sp_tex->u("lp"),-2.5,2,3,1); //Współrzędne źródła światła
    glUniform4f(sp_tex->u("lp2"),2.5,2,3,1); //Współrzędne źródła światła

    glUniform1i(sp_tex->u("textureMap0"),0);
    glActiveTexture(GL_TEXTURE0);

    for(int i = 0; i < 4; i++)
    {
        if(i == 0)
        {
            glBindTexture(GL_TEXTURE_2D,tex4);
            glUniformMatrix4fv(sp_tex->u("M"),1,false,glm::value_ptr(M1));
        }
        if(i == 1)
        {
            glBindTexture(GL_TEXTURE_2D,tex1);
            glUniformMatrix4fv(sp_tex->u("M"),1,false,glm::value_ptr(M2));
        }
        if(i == 2)
        {
            glBindTexture(GL_TEXTURE_2D,tex2);
            glUniformMatrix4fv(sp_tex->u("M"),1,false,glm::value_ptr(M3));
        }
        if(i == 3)
        {
            glBindTexture(GL_TEXTURE_2D,tex3);
            glUniformMatrix4fv(sp_tex->u("M"),1,false,glm::value_ptr(M4));
        }

        drawObject(sp_tex, model1.vertices3, model1.normals3, model1.textures3, model1.vertices4, model1.normals4, model1.textures4);
    }

    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}


//Główna pętla - tu jest generalnie straszny syf
int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

    float arrow_angle_speed = PI/3;
    float arrow_power_speed = 100;
    float gravity = 1.0;
    bool rocket_hit = false;
    float angle = 0;
    float speed = PI / 4;
	glfwSetTime(0); //Zeruj timer

    //Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
        if(stage == 1)
        {
            tank1.CalculateHeight();
            tank2.CalculateHeight();
        }
        if(turn == -1 || (turn == 2 && previous_turn == -1))
        {
            if(combat_mode == 0)
            {
                tank1.angle+=speed_x*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
                tank1.x+=cos(tank1.angle) * speed_y*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
                tank1.z+=sin(tank1.angle) * speed_y*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
            }
            else if(combat_mode == 1)
            {
                if (even_number && arrow.angle >= PI / 2){even_number = false;}
                if (!even_number && arrow.angle <= 0){even_number = true;}
                if (even_number){arrow.angle += arrow_angle_speed * glfwGetTime();}
                if (!even_number){arrow.angle -= arrow_angle_speed * glfwGetTime();}
            }
            else if(combat_mode == 2)
            {
                if (even_number && arrow.power <= 0){even_number = false;}
                if (!even_number && arrow.power >= 100){even_number = true;}
                if (even_number){arrow.power -= arrow_power_speed * glfwGetTime();}
                if (!even_number){arrow.power += arrow_power_speed * glfwGetTime();}
            }
            else if(combat_mode == 3)
            {
                if(previous_combat_mode == 2)
                {
                    rocket.x = tank1.x + 2.0f;
                    rocket.y = tank1.y + 2.0f;
                    rocket.z = tank1.z;
                    rocket.angle_vertical = arrow.angle;
                    rocket.angle_horizontal = tank1.angle;
                    rocket.speed = arrow.power * 0.4;
                    rocket.initialize_speeds();
                    previous_combat_mode = 3;
                    turn = 2;
                    previous_turn = -1;
                }
                else
                {
                    rocket.speed_y -= gravity;
                    rocket.x += glfwGetTime() * rocket.speed_x;
                    rocket.z += glfwGetTime() * rocket.speed_z;
                    rocket.y += glfwGetTime() * rocket.speed_y;
                    rocket.update_angle();
                    if(rocket.y <= 2 + tank1.y && !rocket_hit)
                    {
                        damage.x = rocket.x;
                        damage.z = rocket.z;
                        damage.speed = sqrt(pow(rocket.speed_x, 2) + pow(rocket.speed_y, 2) + pow(rocket.speed_z, 2));
                        tank1.hp -= damage.collision1();
                        if(tank1.hp <= 0)
                        {
                            stage = 2;
                            winner = 1;
                        }
                        if(damage.collision1() > 0){rocket_hit = true;}
                    }
                    if(rocket.y <= 2 + tank2.y && !rocket_hit)
                    {
                        damage.x = rocket.x;
                        damage.z = rocket.z;
                        damage.speed = sqrt(pow(rocket.speed_x, 2) + pow(rocket.speed_y, 2) + pow(rocket.speed_z, 2));
                        tank2.hp -= damage.collision2();
                        if(tank2.hp <= 0)
                        {
                            stage = 2;
                            winner = -1;
                        }
                        if(damage.collision2() > 0){rocket_hit = true;}
                    }
                    if(rocket_hit)
                    {
                        combat_mode = 4;
                        start = clock();
                        while((clock() - start) / (double)(CLOCKS_PER_SEC) < 2)
                        {
                            for(int i = 0; i < 10000; i++) explosion[i].update();
                            drawScene(window, rocket.x, rocket.z);
                        }

                        turn = 1;
                        previous_turn = -1;
                        combat_mode = 0;
                        rocket_hit = false;
                        wind.randomise();

                        for(int i = 0; i < 10000; i++) explosion[i].time_to_live = 20;
                        hit = 0;
                    }
                    else if(rocket.y <= 0.0)
                    {
                        combat_mode = 4;
                        start = clock();
                        while((clock() - start) / (double)(CLOCKS_PER_SEC) < 2){drawScene(window, 0, 0);}

                        turn = 1;
                        previous_turn = -1;
                        combat_mode = 0;
                        wind.randomise();
                    }
                }
            }
        }
        else if(turn == 1 || (turn == 2 && previous_turn == 1))
        {
            if(combat_mode == 0)
            {
                tank2.angle-=speed_x*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
                tank2.x-=cos(tank2.angle) * speed_y*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
                tank2.z+=sin(tank2.angle) * speed_y*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
            }
            else if(combat_mode == 1)
            {
                if (even_number && arrow.angle >= PI / 2){even_number = false;}
                if (!even_number && arrow.angle <= 0){even_number = true;}
                if (even_number){arrow.angle += arrow_angle_speed * glfwGetTime();}
                if (!even_number){arrow.angle -= arrow_angle_speed * glfwGetTime();}
            }
            else if(combat_mode == 2)
            {
                if (even_number && arrow.power <= 0){even_number = false;}
                if (!even_number && arrow.power >= 100){even_number = true;}
                if (even_number){arrow.power -= arrow_power_speed * glfwGetTime();}
                if (!even_number){arrow.power += arrow_power_speed * glfwGetTime();}
            }
            else if(combat_mode == 3)
            {
                if(previous_combat_mode == 2)
                {
                    rocket.x = tank2.x - 2.0f;
                    rocket.y = tank2.y + 2.0f;
                    rocket.z = tank2.z;
                    rocket.angle_vertical = arrow.angle;
                    rocket.angle_horizontal = tank2.angle;
                    rocket.speed = arrow.power * 0.3;
                    rocket.initialize_speeds();
                    previous_combat_mode = 3;
                    previous_turn = 1;
                    turn = 2;
                }
                else
                {
                    rocket.speed_y -= gravity;
                    rocket.x -= glfwGetTime() * rocket.speed_x;
                    rocket.z += glfwGetTime() * rocket.speed_z;
                    rocket.y += glfwGetTime() * rocket.speed_y;
                    rocket.update_angle();
                    if(rocket.y <= 2 + tank1.y && !rocket_hit)
                    {
                        damage.x = rocket.x;
                        damage.z = rocket.z;
                        damage.speed = sqrt(pow(rocket.speed_x, 2) + pow(rocket.speed_y, 2) + pow(rocket.speed_z, 2));
                        tank1.hp -= damage.collision1();
                        if(tank1.hp <= 0)
                        {
                            stage = 2;
                            winner = 1;
                        }
                        if(damage.collision1() > 0){rocket_hit = true;}
                    }
                    if(rocket.y <= 2 + tank2.y && !rocket_hit)
                    {
                        damage.x = rocket.x;
                        damage.z = rocket.z;
                        damage.speed = sqrt(pow(rocket.speed_x, 2) + pow(rocket.speed_y, 2) + pow(rocket.speed_z, 2));
                        tank2.hp -= damage.collision2();
                        if(tank2.hp <= 0)
                        {
                            stage = 2;
                            winner = -1;
                        }
                        if(damage.collision2() > 0){rocket_hit = true;}
                    }
                    if(rocket_hit)
                    {
                        combat_mode = 4;
                        start = clock();
                        while((clock() - start) / (double)(CLOCKS_PER_SEC) < 2)
                        {
                            for(int i = 0; i < 10000; i++) explosion[i].update();
                            drawScene(window, rocket.x, rocket.z);
                        }

                        turn = -1;
                        previous_turn = 1;
                        combat_mode = 0;
                        rocket_hit = false;
                        wind.randomise();

                        for(int i = 0; i < 10000; i++) explosion[i].time_to_live = 20;
                        hit = 0;
                    }
                    else if(rocket.y <= 0.0)
                    {
                        combat_mode = 4;
                        start = clock();
                        while((clock() - start) / (double)(CLOCKS_PER_SEC) < 2){drawScene(window, 0, 0);}

                        turn = -1;
                        previous_turn = 1;
                        combat_mode = 0;
                        wind.randomise();
                    }
                }
            }
        }
        if(stage == 0)
        {
            angle += speed * glfwGetTime();
            glfwSetTime(0); //Zeruj timer
            drawChoosingScene(window, angle); //Wykonaj procedurę rysującą
        }
        if(stage == 1)
        {
            glfwSetTime(0); //Zeruj timer
            drawScene(window, 0, 0); //Wykonaj procedurę rysującą
        }
        if(stage == 2)
        {
            angle += speed * glfwGetTime();
            for(int i = 0; i < 100; i++) konfetti[i].update();
            glfwSetTime(0); //Zeruj timer
            drawVictoryScene(window, angle);
        }
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.M1=glm::rotate(M1,-a_angle + PI,glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz modelu
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
