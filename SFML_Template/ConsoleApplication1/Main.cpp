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



    design Frame;
    design background;

int main()
{

    //definitions level1
    design ground[13];
    design Platform;
    design fire_lake[4];
    design water_lake[3];
    design box;
    design triangle;
    design fire_point[4];
    design water_point[4];
    


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //load level1
    Frame.texture.loadFromFile("game_textures/sprite_frame_level0/sprite_frame_level2.20.png");
    background.texture.loadFromFile("game_textures/background/Gemini_Generated_Image_pn65sypn65sypn65.png");
    ground[1].texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground[2].texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground[3].texture.loadFromFile("game_textures\\Tile_56\\sprite_colom0.png");
    ground[4].texture.loadFromFile("game_textures\\Tile_47_1\\sprite_0.png");
    ground[5].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56 (1) (1).png");
    ground[6].texture.loadFromFile("game_textures/Tile_47/sprite_hight_ground0.png");
    ground[7].texture.loadFromFile("game_textures/Tile_48/sprite_minitri0.png");
    ground[8].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56 (1) (1).png");
    ground[9].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground[10].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground[11].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56 (1) (1).png");
    ground[12].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    Platform.texture.loadFromFile("game_textures\\Platform\\sprite_platform2.00.png");
    fire_lake[1].texture.loadFromFile("game_textures/sprite_01/sprite_0.png");
    fire_lake[2].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\sprite_0 (1).png");
    fire_lake[3].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\sprite_0 (1).png");
    water_lake[1].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\6375e845-48fe-4c20-ad50-a5e0cc2a226d (1).png");
    water_lake[2].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\6375e845-48fe-4c20-ad50-a5e0cc2a226d (1).png");
    box.texture.loadFromFile("game_textures\\stone.png");
    triangle.texture.loadFromFile("game_textures\\Triangle\\sprite_tri0.png");
    fire_point[1].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    fire_point[2].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    fire_point[3].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    water_point[1].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
    water_point[2].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
    water_point[3].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
   


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Sprite level1
    Frame.sprite.setTexture(Frame.texture);
    background.sprite.setTexture(background.texture);
    box.sprite.setTexture(box.texture);
    ground[1].sprite.setTexture(ground[1].texture);
    ground[2].sprite.setTexture(ground[2].texture);
    ground[3].sprite.setTexture(ground[3].texture);
    ground[4].sprite.setTexture(ground[4].texture);
    ground[5].sprite.setTexture(ground[5].texture);
    ground[6].sprite.setTexture(ground[6].texture);
    ground[7].sprite.setTexture(ground[7].texture);
    ground[8].sprite.setTexture(ground[8].texture);
    ground[9].sprite.setTexture(ground[9].texture);
    ground[10].sprite.setTexture(ground[10].texture);
    ground[11].sprite.setTexture(ground[11].texture);
    ground[12].sprite.setTexture(ground[12].texture);
    Platform.sprite.setTexture(Platform.texture);
    fire_lake[1].sprite.setTexture(fire_lake[1].texture);
    fire_lake[2].sprite.setTexture(fire_lake[2].texture);
    fire_lake[3].sprite.setTexture(fire_lake[3].texture);
    water_lake[1].sprite.setTexture(water_lake[1].texture);
    water_lake[2].sprite.setTexture(water_lake[2].texture);
    triangle.sprite.setTexture(triangle.texture);
    fire_point[1].sprite.setTexture(fire_point[1].texture);
    fire_point[2].sprite.setTexture(fire_point[2].texture);
    fire_point[3].sprite.setTexture(fire_point[3].texture);
    water_point[1].sprite.setTexture(water_point[1].texture);
    water_point[2].sprite.setTexture(water_point[2].texture);
    water_point[3].sprite.setTexture(water_point[3].texture);



    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Level 1 Design

    //Frame
    Frame.sprite.setScale(1.1, 1.5);
    Frame.sprite.setPosition(-96, -96 - 32);


    //background
    background.sprite.setScale(1.875, 1.5);


    //ground1
    ground[1].sprite.setScale(0.875, 1.5);
    ground[1].sprite.setPosition(0, 1000);


    //// ground2
    ground[2].sprite.setScale(0.875, 1.5);
    ground[2].sprite.setPosition(1220, 1000);


    // ground3
    ground[3].sprite.setScale(3, 1.2);
    ground[3].sprite.setPosition(200, 1000 - 691.2 + 100);


    // ground4
    ground[4].sprite.setPosition(1920 - 105.6 - 182, 700 + 50);


    // ground5
    ground[5].sprite.setScale(0.5, 1);
    ground[5].sprite.setPosition(296, 650 + 50);


    // ground6
    ground[6].sprite.setScale(0.8, 1);
    ground[6].sprite.setPosition(744 - 86.4, 650 - 64 + 50);


    //ground7
    ground[7].sprite.setPosition(1448 - 172.8, 650 + 50);


    //ground8
    ground[8].sprite.setScale(0.5, 1);
    ground[8].sprite.setPosition(296, 1000 - 691.2 + 100);


    //ground9
    ground[9].sprite.setPosition(1920 - 105.6 - 182, 450);


    //ground10
    ground[10].sprite.setPosition(506 - 50, 300);


    //ground11
    ground[11].sprite.setScale(0.5, 1);
    ground[11].sprite.setPosition(932.6, 300);


    //ground12
    ground[12].sprite.setScale(-1, 1);
    ground[12].sprite.setPosition(1545 + 20.6, 300);


    //Platform
    Platform.sprite.setPosition(848, 1080 - 128 - 64);


    // box
    box.sprite.setPosition(1700, 1000 - 64 - 23);


    // Triengle
    triangle.sprite.setScale(2, 2);
    triangle.sprite.setPosition(1448 - 70, 500 + 50);


    // Fire lake 1 
    fire_lake[1].sprite.setPosition(700, 1000);
    fire_lake[1].sprite.setScale(1.82, 2);


    // Fire lake 2 
    fire_lake[2].sprite.setScale(0.7, 2);
    fire_lake[2].sprite.setPosition(1160 - 86.4, 650 + 50);


    // Fire lake 3
    fire_lake[3].sprite.setScale(0.7, 2);
    fire_lake[3].sprite.setPosition(1092.6, 300);


    // Water lake 1
    water_lake[1].sprite.setScale(0.7, 2);
    water_lake[1].sprite.setPosition(456, 650 + 50);


    // Water lake 2
    water_lake[2].sprite.setScale(0.7, 2);
    water_lake[2].sprite.setPosition(731, 300);


    //fire_point1
    fire_point[1].sprite.setScale(0.1, 0.1);
    fire_point[1].sprite.setPosition(880, 856 - 23);


    //fire_point2
    fire_point[2].sprite.setScale(0.1, 0.1);
    fire_point[2].sprite.setPosition(350, 700 - 55);


    //fire_point3
    fire_point[3].sprite.setScale(0.1, 0.1);
    fire_point[3].sprite.setPosition(456 + 137, 300 - 55);

    //water_point1
    water_point[1].sprite.setScale(0.1, 0.1);
    water_point[1].sprite.setPosition(950, 856 - 23);


    //water_point2
    water_point[2].sprite.setScale(0.1, 0.1);
    water_point[2].sprite.setPosition(865.6, 600 - 22);


    //water_point3
    water_point[3].sprite.setScale(0.1, 0.1);
    water_point[3].sprite.setPosition(932.6 + 80, 300 - 55);





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
        window.draw(ground[3].sprite);
        window.draw(ground[1].sprite);
        window.draw(ground[2].sprite);
        window.draw(ground[4].sprite);
        window.draw(ground[5].sprite);
        window.draw(ground[6].sprite);
        window.draw(ground[7].sprite);
        window.draw(ground[8].sprite);
        window.draw(ground[9].sprite);
        window.draw(ground[10].sprite);
        window.draw(ground[11].sprite);
        window.draw(ground[12].sprite);
        window.draw(Platform.sprite);
        window.draw(fire_lake[1].sprite);
        window.draw(fire_lake[2].sprite);
        window.draw(fire_lake[3].sprite);
        window.draw(water_lake[1].sprite);
        window.draw(water_lake[2].sprite);
        window.draw(box.sprite);
        window.draw(triangle.sprite);
        window.draw(fire_point[1].sprite);
        window.draw(fire_point[2].sprite);
        window.draw(fire_point[3].sprite);
        window.draw(water_point[1].sprite);
        window.draw(water_point[2].sprite);
        window.draw(water_point[3].sprite);
        window.display();
    }
    return 0;
}