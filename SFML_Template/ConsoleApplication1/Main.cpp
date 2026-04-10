#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <SFML/Audio.hpp>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace sf;

struct design
{
    Texture texture;
    Sprite sprite;
};


int main()
{

    //definitions
    design Frame;
    design background;
    design ground1;
    design ground2;
    design ground3;
    design ground4;
    design ground5;
    design ground6;
    design ground7;
    design ground8;
    design ground9;
    design ground10;
    design ground11;
    design ground12;
    design Platform;
    design fire_lake1;
    design fire_lake2;
    design fire_lake3;
    design water_lake1;
    design water_lake2;
    design box;
    design triangle;
    design fire_point1;
    design fire_point2;
    design fire_point3;
    design water_point1;
    design water_point2;
    design water_point3;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //load
    Frame.texture.loadFromFile("game_textures/sprite_frame_level0/sprite_frame_level2.20.png");
    background.texture.loadFromFile("game_textures/background/Gemini_Generated_Image_pn65sypn65sypn65.png");
    ground1.texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground2.texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground3.texture.loadFromFile("game_textures\\Tile_56\\sprite_colom0.png");
    ground4.texture.loadFromFile("game_textures\\Tile_47 (1)\\sprite_0.png");
    ground5.texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56 (1) (1).png");
    ground6.texture.loadFromFile("game_textures/Tile_47/sprite_hight_ground0.png");
    ground7.texture.loadFromFile("game_textures/Tile_48/sprite_minitri0.png");
    ground8.texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56 (1) (1).png");
    ground9.texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground10.texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground11.texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56 (1) (1).png");
    ground12.texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    Platform.texture.loadFromFile("game_textures\\Platform\\sprite_platform2.00.png");
    fire_lake1.texture.loadFromFile("game_textures/sprite_01/sprite_0.png");
    fire_lake2.texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\sprite_0 (1).png");
    fire_lake3.texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\sprite_0 (1).png");
    water_lake1.texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\6375e845-48fe-4c20-ad50-a5e0cc2a226d (1).png");
    water_lake2.texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\6375e845-48fe-4c20-ad50-a5e0cc2a226d (1).png");
    box.texture.loadFromFile("game_textures\\stone.png");
    triangle.texture.loadFromFile("game_textures\\Triangle\\sprite_tri0.png");
    fire_point1.texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    fire_point2.texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    fire_point3.texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    water_point1.texture.loadFromFile("game_textures\\Water Spell_Frame_03\\Water Spell_Frame_03.png");
    water_point2.texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
    water_point3.texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
  

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Sprite
    Frame.sprite.setTexture(Frame.texture);
    background.sprite.setTexture(background.texture);
    box.sprite.setTexture(box.texture);
    ground1.sprite.setTexture(ground1.texture);
    ground2.sprite.setTexture(ground2.texture);
    ground3.sprite.setTexture(ground3.texture);
    ground4.sprite.setTexture(ground4.texture);
    ground5.sprite.setTexture(ground5.texture);
    ground6.sprite.setTexture(ground6.texture);
    ground7.sprite.setTexture(ground7.texture);
    ground8.sprite.setTexture(ground8.texture);
    ground9.sprite.setTexture(ground9.texture);
    ground10.sprite.setTexture(ground10.texture);
    ground11.sprite.setTexture(ground11.texture);
    ground12.sprite.setTexture(ground12.texture);
    Platform.sprite.setTexture(Platform.texture);
    fire_lake1.sprite.setTexture(fire_lake1.texture);
    fire_lake2.sprite.setTexture(fire_lake2.texture);
    fire_lake3.sprite.setTexture(fire_lake3.texture);
    water_lake1.sprite.setTexture(water_lake1.texture);
    water_lake2.sprite.setTexture(water_lake2.texture);
    triangle.sprite.setTexture(triangle.texture);
    fire_point1.sprite.setTexture(fire_point1.texture);
    fire_point2.sprite.setTexture(fire_point2.texture);
    fire_point3.sprite.setTexture(fire_point3.texture);
    water_point1.sprite.setTexture(water_point1.texture);
    water_point2.sprite.setTexture(water_point2.texture);
    water_point3.sprite.setTexture(water_point3.texture);
 
    

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Level 1 Design

    //Frame
    Frame.sprite.setScale(1.1, 1.5);
    Frame.sprite.setPosition(-96, -96 - 32);


    //background
    background.sprite.setScale(1.875, 1.5);


    //ground1
    ground1.sprite.setScale(0.875, 1.5);
    ground1.sprite.setPosition(0, 1000);


    //// ground2
    ground2.sprite.setScale(0.875, 1.5);
    ground2.sprite.setPosition(1220, 1000);


    // ground3
    ground3.sprite.setScale(3, 1.2);
   ground3.sprite.setPosition(200, 1000-691.2 +100);


    // ground4
   ground4.sprite.setPosition(1920 - 105.6 - 182 , 700+50);
   
   
    // ground5
   ground5.sprite.setScale(0.5, 1);
   ground5.sprite.setPosition(296, 650+50);


    // ground6
   ground6.sprite.setScale(0.8, 1);
   ground6.sprite.setPosition(744-86.4 , 650-64+50);


   //ground7
   ground7.sprite.setPosition(1448-172.8, 650+50);
   

   //ground8
   ground8.sprite.setScale(0.5, 1);
   ground8.sprite.setPosition(296, 1000 - 691.2 +100);
   

   //ground9
   ground9.sprite.setPosition(1920 - 105.6 - 182, 450);


   //ground10
   ground10.sprite.setPosition(506-50, 300);


   //ground11
   ground11.sprite.setScale(0.5, 1);
   ground11.sprite.setPosition(932.6, 300);


   //ground12
   ground12.sprite.setScale(-1, 1);
   ground12.sprite.setPosition(1545+20.6, 300);


    //Platform
    Platform.sprite.setPosition(848, 1080 - 128 - 64);


    // box
    box.sprite.setPosition(1700, 1000 - 64 - 23);


    // Triengle
    triangle.sprite.setScale(2, 2);
    triangle.sprite.setPosition(1448 - 70, 500+50);
   

    // Fire lake 1 
    fire_lake1.sprite.setPosition(700, 1000);
    fire_lake1.sprite.setScale(1.82, 2);


    // Fire lake 2 
    fire_lake2.sprite.setScale(0.7, 2);
    fire_lake2.sprite.setPosition(1160 - 86.4, 650+50);


    // Fire lake 3
    fire_lake3.sprite.setScale(0.7, 2);
    fire_lake3.sprite.setPosition(1092.6, 300);


    // Water lake 1
    water_lake1.sprite.setScale(0.7, 2);
    water_lake1.sprite.setPosition(456, 650+50);


    // Water lake 2
    water_lake2.sprite.setScale(0.7, 2);
    water_lake2.sprite.setPosition(731, 300);


    //fire_point1
    fire_point1.sprite.setScale(0.1, 0.1);
    fire_point1.sprite.setPosition(880, 856-23);


    //fire_point2
    fire_point2.sprite.setScale(0.1, 0.1);
    fire_point2.sprite.setPosition(350, 700 - 55);


    //fire_point3
    fire_point3.sprite.setScale(0.1, 0.1);
    fire_point3.sprite.setPosition(456 + 137,300-55);

    //water_point1
    water_point1.sprite.setScale(0.1, 0.1);
    water_point1.sprite.setPosition(950, 856 - 23);


    //water_point2
    water_point2.sprite.setScale(0.1, 0.1);
    water_point2.sprite.setPosition(865.6, 600 - 22);


    //water_point3
    water_point3.sprite.setScale(0.1, 0.1);
    water_point3.sprite.setPosition(932.6 + 80, 300 - 55);



    RenderWindow window = { VideoMode(1920,1080),"sfml works" };
    Event event;
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        



        window.clear();
        window.draw(background.sprite);
        window.draw(Frame.sprite);
        window.draw(ground3.sprite);
        window.draw(ground1.sprite);
        window.draw(ground2.sprite);
        window.draw(ground4.sprite);
        window.draw(ground5.sprite);
        window.draw(ground6.sprite);
        window.draw(ground7.sprite);
        window.draw(ground8.sprite);
        window.draw(ground9.sprite);
        window.draw(ground10.sprite);
        window.draw(ground11.sprite);
        window.draw(ground12.sprite);
        window.draw(Platform.sprite);
        window.draw(fire_lake1.sprite);
        window.draw(fire_lake2.sprite);
        window.draw(fire_lake3.sprite);
        window.draw(water_lake1.sprite);
        window.draw(water_lake2.sprite);
        window.draw(box.sprite);
        window.draw(triangle.sprite);
        window.draw(fire_point1.sprite);
        window.draw(fire_point2.sprite);
        window.draw(fire_point3.sprite);
        window.draw(water_point1.sprite);
        window.draw(water_point2.sprite);
        window.draw(water_point3.sprite);
        window.display();
    }
    return 0;
}