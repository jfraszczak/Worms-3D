#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

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

    private:
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
      float pom;
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
                vertices.push_back(str2f(splitted));
            }
        }
        else if(line[0] == 'v' && line[1] == 't')
        {
            for(int i = 1; i < split_line(line).size(); i++)
            {
                splitted = split_line(line)[i];
                textures.push_back(str2f(splitted));
            }
        }
        else if(line[0] == 'v' && line[1] == 'n')
        {
            for(int i = 1; i < split_line(line).size(); i++)
            {
                splitted = split_line(line)[i];
                normals.push_back(str2f(splitted));
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
                            index_vert_czworo.push_back(str2int(pom) - 1);
                        }
                        else
                        {
                            index_vert_troj.push_back(str2int(pom) - 1);
                        }
                    }
                    if(licznik == 1)
                    {
                        if(n > 9)
                        {
                            index_tex_czworo.push_back(str2int(pom) - 1);
                        }
                        else
                        {
                            index_tex_troj.push_back(str2int(pom) - 1);
                        }
                    }
                    if(licznik == 2)
                    {
                        if(n > 9)
                        {
                            index_norm_czworo.push_back(str2int(pom) - 1);
                        }
                        else
                        {
                            index_norm_troj.push_back(str2int(pom) - 1);
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
                index_norm_czworo.push_back(str2int(pom) - 1);
            }
            else
            {
                index_norm_troj.push_back(str2int(pom) - 1);
            }
        }
      }
      for(int i = 0; i < index_vert_troj.size(); i++)
      {
          for(int j = 0; j < 3; j++)
          {
              cout<<vertices[3*index_vert_troj[i] + j]<<endl;
              vertices3.push_back(vertices[3*index_vert_troj[i] + j]);
              textures3.push_back(textures[3*index_tex_troj[i] + j]);
              normals3.push_back(normals[3*index_norm_troj[i] + j]);
          }
      }
      for(int i = 0; i < index_vert_czworo.size(); i++)
      {
          for(int j = 0; j < 3; j++)
          {
              vertices4.push_back(vertices[3*index_vert_czworo[i] + j]);
              textures4.push_back(textures[3*index_tex_czworo[i] + j]);
              normals4.push_back(normals[3*index_norm_czworo[i] + j]);
          }
      }
      myfile.close();
    }
};
