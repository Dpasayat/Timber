#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<stdio.h>
#include<sstream>
#include<SFML/Audio.hpp>

using namespace sf;

void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

//where is the player/branch?
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

//prepare the sounds
SoundBuffer chopBuffer;
chopBuffer.loadFromFile("./sound/chop.wav");
Sound chop;
chop.setBuffer(chopBuffer);

// The player has met his end under a branch
SoundBuffer deathBuffer;
deathBuffer.loadFromFile("./sound/death.wav");
Sound death;
death.setBuffer(deathBuffer);

// Out of time
SoundBuffer ootBuffer;
ootBuffer.loadFromFile("./sound/out_of_time.wav");
Sound outOfTime;
outOfTime.setBuffer(ootBuffer);


int main()
{
	//create a video mode object
	VideoMode vm(1920, 1080);
	//create and open a window for the game
	RenderWindow window(vm, "Timber");

	//create a texture to hold a graphic on the GPU
	Texture textureBackground;
	Texture textureTree;
	Texture textureBee;
	Texture textureCloud;

	//load a graphic into the texture
	textureBackground.loadFromFile("./graphics/background.png");
	textureTree.loadFromFile("./graphics/tree.png");
	textureBee.loadFromFile("./graphics/bee.png");
	textureCloud.loadFromFile("./graphics/cloud.png");
	


	//create a sprite
	Sprite spriteBackground;
	Sprite spriteTree;
	Sprite spriteBee;
	Sprite spriteCloud[3];
	//attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);
	spriteTree.setTexture(textureTree);
	spriteBee.setTexture(textureBee);
	//clouds
	spriteCloud[0].setTexture(textureCloud);
	spriteCloud[1].setTexture(textureCloud);
	spriteCloud[2].setTexture(textureCloud);
	

	//set position of the screen
	spriteBackground.setPosition(0, 0);
	spriteTree.setPosition(1920 / 2 - 300 / 2, 0);
	spriteBee.setPosition(0, 800);

	//clouds
	spriteCloud[0].setPosition(110, 0);
	spriteCloud[1].setPosition(110, 250);
	spriteCloud[2].setPosition(110, 500);
	//are clouds in screen?
	bool cloudActive[3] = { false, false, false };
	//how fast can the clouds move
	float cloudSpeed[3] = { 0.0f, 0.0f, 0.0f };

	//is bee currently moving?
	bool beeActive = false;

	//how fast can the bee fly
	float beeSpeed = 0.0f;

	//Variable to control time itself
	Clock clock;

	//Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition(1920 / 2.0f - timeBarStartWidth / 2.0f, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;




	//Track whether the game is running
	bool paused = true;

	// Draw some text
	int score = 0;

	Text messageText;
	Text scoreText;

	//choose font
	Font font;
	font.loadFromFile("./fonts/KOMIKAP_.ttf");

	//set font
	messageText.setFont(font);
	scoreText.setFont(font);

	//assign message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");
	//set character size
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);
	//set color
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	//position the text
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

	messageText.setPosition(1920 / 2, 1080 / 2);
	scoreText.setPosition(20, 20);

	//prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("./graphics/branch.png");
	for (int i = 0; i < NUM_BRANCHES; i++)
	{
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-200,-200);
		//set the sprite origin to its center. we can spin it round withoud changing its position
		branches[i].setOrigin(220, 20);
	}


	//prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("./graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);
	//player starts from the left
	side playerSide = side::LEFT;
	// Prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	// Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	// Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	// Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);
	// Some other useful log related variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	//control the player input
	bool acceptInput = false;
	
	while (window.isOpen())
	{

		/**********************Handle Player input************************/
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
			{
				acceptInput = true;

				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
		if (Keyboard::isKeyPressed(Keyboard::Return))
		{
			paused = false;
			score = 0;
			timeRemaining = 6;

			//hide the branches from 2nd one
			for (int i = 1; i < NUM_BRANCHES; i++)
			{
				branchPositions[i] = side::NONE;
			}
			// Make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);
			// Move the player into position
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}
		//player inputs
		if (acceptInput)
		{
			//right
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				playerSide = side::RIGHT;
				score++;
				//add time 
				timeRemaining += (2.0f / score) + 0.15;
				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);

				spritePlayer.setPosition(1200, 720);
				// Update the branches
				updateBranches(score);
				// Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;
				acceptInput = false;

				chop.play();

			}
			//left
			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				// Make sure the player is on the left
				playerSide = side::LEFT;
				score++;
				// Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;
				spriteAxe.setPosition(AXE_POSITION_LEFT,
					spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);
				// update the branches
				updateBranches(score);
				// set the log flying
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;

				chop.play();
			}
		}
		/**********************Update the Scene***********************/
		
		if (!paused) 
		{
			//Measure the time
			Time dt = clock.restart();

			//Subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			//size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));


			if (timeRemaining <= 0.0f)
			{
				//pause the game
				paused = true;

				//change thee message 
				messageText.setString("Out of time!!!");
				//recenter the text
				messageText.setOrigin(
					textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f
				);

				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
				outOfTime.play();
			}
			//setup the bee
			if (!beeActive)
			{
				// how fast is the bee flying
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 200;

				//how high is the bee
				srand((int)time(0) * 10);
				int height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;
			}
			else
			{
				//move the bee
				spriteBee.setPosition(
					spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),
					spriteBee.getPosition().y
				);

				//has the bee left the screen?
				if (spriteBee.getPosition().x < -100)
				{
					beeActive = false;
				}
			}
			//manage the clouds
			for (int i = 0; i < 3; i++)
			{
				if (!cloudActive[i])
				{
					//setup the cloud
					srand((int)time(0) * (i + 1) * 10);
					cloudSpeed[i] = (rand() % 200);
					float height = (rand() % 150);
					spriteCloud[i].setPosition(-200, height);
					cloudActive[i] = true;
				}
				else
				{
					spriteCloud[i].setPosition(
						spriteCloud[i].getPosition().x + (cloudSpeed[i] * dt.asSeconds()),
						spriteCloud[i].getPosition().y
					);
					//has the cloud left the screen?
					if (spriteCloud[i].getPosition().x > 1920)
					{
						cloudActive[i] = false;
					}
				}
			}

			//update the score
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			//update the branches
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				float height = i * 150;
				if (branchPositions[i] == side::LEFT)
				{
					branches[i].setPosition(610, height);
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT)
				{
					branches[i].setPosition(1300, height);
					branches[i].setRotation(0);
				}
				else
				{
					branches[i].setPosition(3000, height);
				}
			}
			//handle flying log
			if (logActive)
			{
				spriteLog.setPosition(spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()));

				if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x>2000)
				{
					// Set it up ready to be a whole new log next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			//has player been sqished?
			if (branchPositions[5] == playerSide)
			{
				//death
				paused = true;
				acceptInput = false;
				// Draw the gravestone
				spriteRIP.setPosition(525, 760);
				// hide the player
				spritePlayer.setPosition(2000, 660);
				// Change the text of the message
				messageText.setString("SQUISHED!!");
				// Center it on the screen

				FloatRect textRect = messageText.getLocalBounds();


				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);


				messageText.setPosition(1920 / 2.0f,
					1080 / 2.0f);
				death.play();
			}
		}//end if(!paused)


		/**********************Draw the scene************************/
		window.clear();
		//Draw game scene
		window.draw(spriteBackground);
		
		//draw clouds
		
		for(int i = 0; i < 3;i++)	window.draw(spriteCloud[i]);

		window.draw(spriteTree);
		for (auto i: branches)
		{
			window.draw(i);
		}
		// Draw the player
		window.draw(spritePlayer);
		// Draw the axe
		window.draw(spriteAxe);
		// Draw the flying log
		window.draw(spriteLog);
		// Draw the gravestone
		window.draw(spriteRIP);
		
		//draw bee
		window.draw(spriteBee);
		//draw the score
		window.draw(scoreText);
		//draw the message
		if (paused)
		{
			window.draw(messageText);
		}
		//draw the time bar
		window.draw(timeBar);

		//show everything
		window.display();
	}

	return 0;
}

void updateBranches(int seed)
{
	for (int i = 5; i > 0; i--)
	{
		branchPositions[i] = branchPositions[i - 1];
	}
	// spawn new branch
	srand((int)time(0) + seed);
	int r = rand() % 5;
	switch(r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;

	}
}