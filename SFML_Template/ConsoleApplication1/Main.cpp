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

struct ground
{
    Texture texture;
    Sprite sprite;
};

int main()
{

    //Texture
    Texture backgroundTexture;
    ground ground1;
    ground ground2;
    ground ground3;
    ground Platform;
    Texture Fire_bondTexture;
    Texture Fireboy_gateTexture;
    Texture Watergirl_gateTexture;
    Texture Water_bondTexture;
    Texture FrameTexture;

    //load
    backgroundTexture.loadFromFile("game_textures/background/Gemini_Generated_Image_pn65sypn65sypn65.png");
    ground1.texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground2.texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground3.texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    Platform.texture.loadFromFile("game_textures\\Platform\\sprite_platform2.00.png");
    Fire_bondTexture.loadFromFile("game_textures/sprite_0 (1)/sprite_0.png");
    Fireboy_gateTexture.loadFromFile("ground.jpeg");
    Watergirl_gateTexture.loadFromFile("ground.jpeg");
    Water_bondTexture.loadFromFile("ground.jpeg");
    FrameTexture.loadFromFile("game_textures/sprite_frame_level0/sprite_frame_level2.20.png");

    //Sprite
    Sprite frame;
    Sprite background;
    ground1.sprite.setTexture(ground1.texture);
    ground2.sprite.setTexture(ground2.texture);
    ground3.sprite.setTexture(ground3.texture);
    Platform.sprite.setTexture(Platform.texture);
    //Level 1 Design

    //Frame
    frame.setTexture(FrameTexture);
    frame.setScale(1.1, 1.5);
    frame.setPosition(-96, -96 - 32);

    //background
    background.setTexture(backgroundTexture);
    background.setScale(1.875, 1.5);


    //ground1
    ground1.sprite.setScale(0.875, 1.5);
    ground1.sprite.setPosition(0, 1000);

    //// ground2
    ground2.sprite.setScale(0.875, 1.5);
    ground2.sprite.setPosition(1220, 1000);

    //// ground3
    //Sprite ground3;
    //ground3.setTexture(groundTexture);

    //// ground4
    //Sprite ground4;
    //ground4.setTexture(groundTexture);

    //// ground5
    //Sprite ground5;
    //ground5.setTexture(groundTexture);

    //// ground6
    //Sprite ground6;
    //ground6.setTexture(groundTexture);

    //Platform
    Platform.sprite.setPosition(848, 1000 - 128);

    // box
    Texture boxTexture;
    boxTexture.loadFromFile("ground.jpeg");
    Sprite box;
    box.setTexture(boxTexture);

    // Triengle
    Texture TriengleTexture;
    TriengleTexture.loadFromFile("ground.jpeg");
    Sprite Triengle;
    Triengle.setTexture(TriengleTexture);

    // Fire Boy Gate
    Sprite Fireboy_gate;
    Fireboy_gate.setTexture(Fireboy_gateTexture);

    // Water Girl Gate
    Sprite Watergirl_gate;
    Watergirl_gate.setTexture(Watergirl_gateTexture);

    // Fire Bond 1 
    Sprite Fire_bond1;
    Fire_bond1.setTexture(Fire_bondTexture);
    Fire_bond1.setPosition(700, 1000);
    Fire_bond1.setScale(1.82, 1);

    // Fire Bond 2 
    Sprite Fire_bond2;
    Fire_bond2.setTexture(Fire_bondTexture);

    // Fire Bond 3
    Sprite Fire_bond3;
    Fire_bond3.setTexture(Fire_bondTexture);

    // Water Bond 1
    Sprite Water_bond1;
    Water_bond1.setTexture(Water_bondTexture);

    // Water Bond 2
    Sprite Water_bond2;
    Water_bond2.setTexture(Water_bondTexture);

    // Water Bond 3
    Sprite Water_bond3;
    Water_bond3.setTexture(Water_bondTexture);

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
        window.draw(background);
        window.draw(frame);
        window.draw(ground1.sprite);
        window.draw(ground2.sprite);
        window.draw(Platform.sprite);
        window.draw(Fire_bond1);
        window.display();
    }
    return 0;
}