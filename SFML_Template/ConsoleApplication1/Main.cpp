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


struct character {
    const int frameWidth = 160;
    int frameHeight; //boy 140, girl 128 

    Sprite sprite;
    void sprite_origin() {
        sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
        sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
    }

    float speed_y;
    float speed_x;
    float gravity;
    float jump_strength;
    float delay;
    float timer;

    bool onground;
    bool ismoving;
    bool stop;

    int framecounter;
    int totalFrames;

    Keyboard::Key left;
    Keyboard::Key right;
    Keyboard::Key up;
};



void ground_collision(character& player, RectangleShape& ground) {
    if (player.sprite.getGlobalBounds().top + player.sprite.getGlobalBounds().height >= ground.getPosition().y) {
        player.sprite.setPosition(player.sprite.getPosition().x, ground.getPosition().y - (player.frameHeight / 2.0));
        player.speed_y = 0;
        player.onground = true;
    }
    else {
        player.onground = false;
    }
}
void platform_collision(character& player, RectangleShape& ground) {
    FloatRect hitbox = player.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
    if (hitbox.intersects(ground.getGlobalBounds())) {
        float playerBottom = hitbox.top + hitbox.height;
        float platformTop = ground.getPosition().y;
        if (player.speed_y > 0 && playerBottom <= platformTop + 15.0f) {
            player.sprite.setPosition(player.sprite.getPosition().x, ground.getPosition().y - (player.frameHeight / 2.0));
            player.speed_y = 0;
            player.onground = true;
        }
        if (player.speed_y < 0 && hitbox.top >= platformTop - 15.0f) {
            player.sprite.setPosition(player.sprite.getPosition().x, ground.getPosition().y + (ground.getSize().y) + (player.frameHeight / 2.0));
            player.speed_y = 0;
        }
    }
}

void slope_collision(character& player, double x1, double y1, double x2, double y2) {
    double player_x = player.sprite.getPosition().x;
    if (player_x >= x1 && player_x <= x2) {
        double slopeY = y1 + (player_x - x1) * ((y2 - y1) / (x2 - x1));
        if ((player.sprite.getPosition().y + (player.frameHeight / 2.0f)) >= slopeY) {
            player.sprite.setPosition(player_x, slopeY - (player.frameHeight / 2.0f));
            player.speed_y = 0;
            player.onground = true;
        }
    }
}

void wall_collision(character& player, RectangleShape& wall) {
    if (player.sprite.getGlobalBounds().intersects(wall.getGlobalBounds())) {
        if (player.sprite.getPosition().x < wall.getPosition().x && player.sprite.getScale().x == 1) {
            player.stop = true;
        }
        else if ((player.sprite.getPosition().x + wall.getSize().x) > wall.getPosition().x && player.sprite.getScale().x == -1) {
            player.stop = true;

        }
        else {
            player.stop = false;
        }
    }
    if (player.stop) {
        player.speed_x = 0;
    }
    else {
        player.speed_x = 450.0f;
    }
}


void origin(RectangleShape& x)
{
    x.setOrigin(x.getLocalBounds().width / 2, x.getLocalBounds().height / 2);
}

design Frame;
design background;

int main()
{

    //definitions level1
    design ground[12];
    design Platform;
    design fire_lake[3];
    design water_lake[2];
    design box;
    design triangle;
    design fire_point[3];
    design water_point[3];


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //load level1
    Frame.texture.loadFromFile("game_textures/sprite_frame_level0/sprite_frame_level2.20.png");
    background.texture.loadFromFile("game_textures/background/Gemini_Generated_Image_pn65sypn65sypn65.png");
    ground[0].texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground[1].texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground[2].texture.loadFromFile("game_textures\\Tile_56\\sprite_colom0.png");
    ground[3].texture.loadFromFile("game_textures\\Tile_47_1\\sprite_0.png");
    ground[4].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56_1_1.png");
    ground[5].texture.loadFromFile("game_textures/Tile_47/sprite_hight_ground0.png");
    ground[6].texture.loadFromFile("game_textures/Tile_48/sprite_minitri0.png");
    ground[7].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56_1_1.png");
    ground[8].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground[9].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground[10].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56_1_1.png");
    ground[11].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    Platform.texture.loadFromFile("game_textures\\Platform\\sprite_platform2.00.png");
    fire_lake[0].texture.loadFromFile("game_textures/sprite_01/sprite_0.png");
    fire_lake[1].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\sprite_0_1.png");
    fire_lake[2].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\sprite_0_1.png");
    water_lake[0].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\6375e845-48fe-4c20-ad50-a5e0cc2a226d_1.png");
    water_lake[1].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\6375e845-48fe-4c20-ad50-a5e0cc2a226d_1.png");
    box.texture.loadFromFile("game_textures\\stone.png");
    triangle.texture.loadFromFile("game_textures\\Triangle\\sprite_tri0.png");
    fire_point[0].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    fire_point[1].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    fire_point[2].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    water_point[0].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
    water_point[1].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
    water_point[2].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Sprite level1
    Frame.sprite.setTexture(Frame.texture);
    background.sprite.setTexture(background.texture);
    box.sprite.setTexture(box.texture);
    for (int i = 0; i < 12; i++)
        ground[i].sprite.setTexture(ground[i].texture);


    Platform.sprite.setTexture(Platform.texture);

    for (int i = 0; i < 3; i++)
        fire_lake[i].sprite.setTexture(fire_lake[i].texture);

    for (int i = 0; i < 2; i++)
        water_lake[i].sprite.setTexture(water_lake[i].texture);

    triangle.sprite.setTexture(triangle.texture);
    for (int i = 0; i < 3; i++)
        fire_point[i].sprite.setTexture(fire_point[i].texture);

    for (int i = 0; i < 3; i++)
        water_point[i].sprite.setTexture(water_point[i].texture);



    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Level 1 Design

    //Frame
    Frame.sprite.setScale(1.1, 1.5);
    Frame.sprite.setPosition(-96, -96 - 32);


    //background
    background.sprite.setScale(1.875, 1.5);


    //ground1
    ground[0].sprite.setScale(0.875, 1.5);
    ground[0].sprite.setPosition(0, 1000);


    //// ground2
    ground[1].sprite.setScale(0.875, 1.5);
    ground[1].sprite.setPosition(1220, 1000);


    // ground3
    ground[2].sprite.setScale(3, 1.2);
    ground[2].sprite.setPosition(200, 1000 - 691.2 + 100);


    // ground4
    ground[3].sprite.setPosition(1920 - 105.6 - 182, 700 + 50);


    // ground5
    ground[4].sprite.setScale(0.5, 1);
    ground[4].sprite.setPosition(296, 650 + 50);


    // ground6
    ground[5].sprite.setScale(0.8, 1);
    ground[5].sprite.setPosition(744 - 86.4, 650 - 64 + 50);


    //ground7
    ground[6].sprite.setPosition(1448 - 172.8, 650 + 50);


    //ground8
    ground[7].sprite.setScale(0.5, 1);
    ground[7].sprite.setPosition(296, 1000 - 691.2 + 100);


    //ground9
    ground[8].sprite.setPosition(1920 - 105.6 - 182, 450);


    //ground10
    ground[9].sprite.setPosition(506 - 50, 300);


    //ground11
    ground[10].sprite.setScale(0.5, 1);
    ground[10].sprite.setPosition(932.6, 300);


    //ground12
    ground[11].sprite.setScale(-1, 1);
    ground[11].sprite.setPosition(1545 + 20.6, 300);


    //Platform
    Platform.sprite.setPosition(848, 1080 - 128 - 64);


    // box
    box.sprite.setPosition(1700, 1000 - 64 - 23);


    // Triengle
    triangle.sprite.setScale(2, 2);
    triangle.sprite.setPosition(1448 - 70, 500 + 50);


    // Fire lake 1 
    fire_lake[0].sprite.setPosition(700, 1000);
    fire_lake[0].sprite.setScale(1.82, 2);


    // Fire lake 2 
    fire_lake[1].sprite.setScale(0.7, 2);
    fire_lake[1].sprite.setPosition(1160 - 86.4, 650 + 50);


    // Fire lake 3
    fire_lake[2].sprite.setScale(0.7, 2);
    fire_lake[2].sprite.setPosition(1092.6, 300);


    // Water lake 1
    water_lake[0].sprite.setScale(0.7, 2);
    water_lake[0].sprite.setPosition(456, 650 + 50);


    // Water lake 2
    water_lake[1].sprite.setScale(0.7, 2);
    water_lake[1].sprite.setPosition(731, 300);


    //fire_point1
    fire_point[0].sprite.setScale(0.1, 0.1);
    fire_point[0].sprite.setPosition(880, 856 - 23);


    //fire_point2
    fire_point[1].sprite.setScale(0.1, 0.1);
    fire_point[1].sprite.setPosition(350, 700 - 55);


    //fire_point3
    fire_point[2].sprite.setScale(0.1, 0.1);
    fire_point[2].sprite.setPosition(456 + 137, 300 - 55);

    //water_point1
    water_point[0].sprite.setScale(0.1, 0.1);
    water_point[0].sprite.setPosition(950, 856 - 23);


    //water_point2
    water_point[1].sprite.setScale(0.1, 0.1);
    water_point[1].sprite.setPosition(865.6, 600 - 22);


    //water_point3
    water_point[2].sprite.setScale(0.1, 0.1);
    water_point[2].sprite.setPosition(932.6 + 80, 300 - 55);



    RenderWindow window = { VideoMode(1920,1080),"sfml works" };
    Event event;
    Clock clock;
    clock.restart();

    RectangleShape ground1(Vector2f(1950, 80));
    ground1.setFillColor(Color::Cyan);
    ground1.setPosition(0, 1000);
    

    character fireboy;
    fireboy.frameHeight = 140;
    Texture boytex;
    if (!boytex.loadFromFile("game_textures\\fireboy_run.png")) {
        cout << "Error: Could not load fireboy image! Check the path";
    }
    fireboy.sprite.setTexture(boytex);
    fireboy.sprite_origin();
    fireboy.sprite.setPosition(700, ground1.getPosition().y - (fireboy.frameHeight / 2.0f));
    fireboy.framecounter = 0;
    fireboy.totalFrames = 6; //boy 6, girl 9
    fireboy.delay = 0.1f;
    fireboy.speed_x = 450.0f;
    fireboy.speed_y = 450.0f;
    fireboy.jump_strength = -900.0f;
    fireboy.gravity = 2500.0f;
    fireboy.timer = fireboy.delay;
    fireboy.onground = true;
    fireboy.ismoving = false;
    fireboy.stop = false;
    fireboy.right = Keyboard::Right;
    fireboy.left = Keyboard::Left;
    fireboy.up = Keyboard::Up;

    character watergirl;
    watergirl.frameHeight = 128;
    Texture girltex;
    if (!girltex.loadFromFile("game_textures\\watergirl_run.png")) {
        cout << "Error: Could not load watergirl image! Check the path";
    }
    watergirl.sprite.setTexture(girltex);
    watergirl.sprite_origin();
    watergirl.sprite.setPosition(600, ground1.getPosition().y - (watergirl.frameHeight / 2.0f));
    watergirl.framecounter = 0;
    watergirl.totalFrames = 9; //boy 6, girl 9
    watergirl.delay = 0.1f;
    watergirl.speed_x = 450.0f;
    watergirl.speed_y = 450.0f;
    watergirl.jump_strength = -900.0f;
    watergirl.gravity = 2500.0f;
    watergirl.timer = watergirl.delay;
    watergirl.onground = true;
    watergirl.ismoving = false;
    watergirl.stop = false;
    watergirl.right = Keyboard::D;
    watergirl.left = Keyboard::A;
    watergirl.up = Keyboard::W;




    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }



        window.clear();

        window.draw(ground1);

        window.draw(background.sprite);
        window.draw(Frame.sprite);

        for (int i = 0; i < 12; i++)
            window.draw(ground[i].sprite);

        window.draw(Platform.sprite);

        for (int i = 0; i < 3; i++)
            window.draw(fire_lake[i].sprite);

        for (int i = 0; i < 2; i++)
            window.draw(water_lake[i].sprite);

        window.draw(box.sprite);
        window.draw(triangle.sprite);

       

		for (int i = 0; i < 3; i++)
        window.draw(fire_point[i].sprite);
       
		for (int i = 0; i < 3; i++)
        window.draw(water_point[i].sprite);

        window.draw(fireboy.sprite);
        window.draw(watergirl.sprite);

        window.display();
    }
    return 0;
}