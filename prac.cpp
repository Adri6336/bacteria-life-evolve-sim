// Bug: population spontaneously booms without enough food

#include <SFML/Graphics.hpp>  // Allows me to run the simulation in a window and graphically display it
#include <string>  // Allows me access to the string datatype
#include <random>  // Allows me to semirandomly place things
#include <vector>  // Allows me to store dynamic lists of objects
#include <cmath>  // Allows me to use absolute functions 
#include <fstream>  // Allows me to save a file
#include <algorithm>
#include <iostream>  // Allows me to print to console
#include <unistd.h>  // sleep()
using namespace std;

// Function prototypes  (only add needed ones)
int randInt(int lower, int upper);

// Useful sin (global variables)
int born = 0;  // This is a count of the number of bacteria that spawn during the simulation
const int MAX = 20000;  // Hard coded max population. Avoids seg fault
const int LENGTH = 800;  // Left-to-right size of world
const int WIDTH = 600;  // Up and down size of world 
const int MAX_FOOD = 1000;  // Max amount of food that will spawn
const int REFIL_SEC = 30;  // Time it takes to refil in seconds
const bool RENDER_BUGS = true;  // If false, bugs will not render
const int SIMFOR = 300;  // This sets the length of time in seconds that the sim will run for before comprehensive analysis. -404 = no end


// ====== Classes

class bacteria{
    public:

        // Morphology =======================
        // ==================================

        bacteria()
        {
            bug.setRadius(static_cast<float>(genome.at(1)));  // The size corresponds to the size trait of genome
            move(400, 300);
            sf::Color color(genome.at(5), genome.at(6), genome.at(7));  // This is the genetically defined color. Used for differentiation
            bug.setFillColor(color);
            radius = genome.at(1);  // Set radius info to size
        }

        int getRadius()
        {
            return radius;
        }

        void refreshBacteria()
        {
            bug.setRadius(static_cast<float>(genome.at(1)));  // The size corresponds to the size trait of genome

            sf::Color color(genome.at(5), genome.at(6), genome.at(7));  // This is the genetically defined color. Used for differentiation
            bug.setFillColor(color);
        }

        bool isDead()
        {
            return dead;
        }

        void kill(string cause)
        {
            //cout << "Bacteria killed by " << cause << " at the age of " << age << ". Stomach equalled " << stomach << ".\n";
            dead = true;
        }

        void isTooOld()
        {
            int lifespan = getGene(8);

            if (age >= lifespan)
                kill("old age");
        }

        int getAge()
        {
            return age;
        }

        int getStomach()
        {
            return stomach;
        }

        int getOffspringCount()
        {
            return offspringNum;
        }

        void setCloser(bool closer)
        {
            closerToFood = closer;
        }

        void randomWalk()
        {
            /*
            This simulates the bacterial semi-random walk.
            */
           
            // Variables
            int sel = randInt(0, 7);  // This will determine how it moves (short for selection)
            int changex = 0;
            int changey = 0;
            int xTotal = 0;  // This determines where it'll be after change. Same for one below it
            int yTotal = 0;
            sf::Vector2f whereami = bug.getPosition();
            int speed = genome.at(0);

            // Set movement
            if (sel == 0)  // Up 
            {
                changey -= speed;
            }
            else if (sel == 1)  // Down 
            {
                changey += speed;
            }
            else if (sel == 2) // Left
            {
                changex -= speed;
            }
            else if (sel == 3) // Right
            {
                changex += speed;
            }
            else if (sel == 4) // upleft
            {
                changey -= speed;
                changex -= speed;
            }
            else if (sel == 5) // upright
            {
                changey -= speed;
                changex += speed;
            }
            else if (sel == 6) // downleft
            {
                changey += speed;
                changex -= speed;
            }
            else if (sel == 7) // downright
            {
                changey += speed;
                changex += speed;
            }


            // Prevent bug from exiting the world
            xTotal = whereami.x + changex;
            yTotal = whereami.y + changey;


            if (xTotal > 800)
            {
            xTotal = 800;
            }

            if (xTotal < 0)
            {
            xTotal = 0;
            }

            if (yTotal > 600)
            {
            yTotal = 600;
            }

            if (yTotal < 0)
            {
            yTotal = 0;
            }

            // Remember movement
            memX = changex;  // Just realized that it's not camelcase but I really don't want to rename all that 
            memY = changey;

            // Actualize movement
            move(xTotal, yTotal);

        }

        vector<int> getMemory()
        {
            vector<int> memory(2);
            memory.at(0) = memX;
            memory.at(1) = memY;
            return memory;
        }

        void move(int x, int y)
        {
            if (age < genome.at(8) && !dead)
            {
                bug.setPosition(x, y);
                age++;
            }
            
        }

        void redoLastMove()
        {
            sf::Vector2f myPos = bug.getPosition();

            move(myPos.x + memX, myPos.y + memY);
        }

        sf::Vector2f pos(){
            return bug.getPosition();
        }

        int getGene(int gene)
        {
            /*
            This grabs the gene at a specified place
            */
           return genome.at(gene);

        }

        void setGene(int gene, int value)
        {
            genome.at(gene) = value;
        }

        string getGenomeString()
        {
            /* Returns the genome as a string of form #,#,...#*/
            // Variables
            string textGenome = "";
            int size = genome.size();

            // Iterate through genome and append each gene
            for (int x = 0; x < size; x++)
            {
                textGenome.append(to_string(genome.at(x)));

                if (x < size - 1)  // If its the last gene, don't add a comma
                    textGenome.append(",");
            }

            return textGenome;
        }

        int getGenomeSize()
        {
            /*
            This returns the size of the genome
            */
            return genome.size();
        }

        float lifeLived()
        {
            // This returns a percentage of life lived
            return (static_cast<float>(age) / static_cast<float>(genome.at(8))) * 100;
        }

        // survive ==========================
        // ==================================

        void interpretFoodDistance(int distance)
        {

            // Determine if things are getting better
            if (distance < lastDistance)  // If we're closer to food
            {
                closerToFood = true;
            }
            else 
            {
                closerToFood = false;
            }

            // Remember
            lastDistance = distance;
        }

        void setSeesFood()
        {
            seesFood = true;
        }

        void setSeesNothing()
        {
            seesFood = false;
        }

        bool hasSeenFood()
        {
            return seesFood;
        }

        bool isCloserToFood()
        {
            return closerToFood;
        }

        void goToFood()
        {
            if (closerToFood)
                bug.move(memX, memY);
            
            else
                randomWalk();
        }

        void fillStomach(int food)
        {
            stomach += food;
            seesFood = false; 
        }


        // Reproduce and evolve =============
        // ==================================


        bool canConjugate(bacteria& mate)
        {
            /* 
            Based on how far the mate is. If zero and full, bacteria will try to start conjuration 
            */
            
            // Variables
            //int distance = -1;  The distance will be provided by an outside observer
            sf::Vector2f whereami = pos(); // Th

            // Is it in the mood?
            if (stomach != genome.at(1) || mated || mate.hasConjugated()) 
            {
                return false;  // Bacteria or partner is not in the mood
            }
            else
            {
                return true;  // We already know they're close enough, so we're good to go
            }


        }

        void setConjugated()
        {
            /* Lets the bacteria know it has conjugated */
            mated = true;
        }

        bool hasConjugated()
        {
            /* Tells whether a bacteria has engaged in conjugation before */
            return mated;
        }

        void conjugate(bacteria& mate)
        {
            /* Some genes will randomly be selected. These genes will have their places swapped */

            // Variables
            vector<int> genesPos;
            vector<int> geneVal;
            int genomeSize = getGenomeSize();
            int swapDiv = 2;  // This represents a division. 2 means that half the genome will be swapped.
            int toSwap = floor(genomeSize / swapDiv);  // Round down the division
            int tempSelection = 0;

            // Start selection
            for (int x = 0; x < toSwap; x++)
            {
                if (genesPos.size() == 0)
                {
                    genesPos.push_back(randInt(0, genomeSize - 1));
                    geneVal.push_back(getGene(genesPos.at(0)));
                }
                else
                {
                    tempSelection = randInt(0, genomeSize - 1);
                    while (count(genesPos.begin(), genesPos.end(), tempSelection) > 0)  // If the selection is already made
                        tempSelection = randInt(0, genomeSize - 1);  // Do again until it hasn't

                    genesPos.push_back(tempSelection);
                    geneVal.push_back(getGene(tempSelection));
                }

            }


            // Start incorperation
            for (int x = 0; x < genesPos.size(); x++)
            {
                genome.at(genesPos.at(x)) = mate.getGene(genesPos.at(x));  // Grab mate's gene and incorperate into genome
                mate.setGene(genesPos.at(x), geneVal.at(x));  // Put gene into mate's genome 
            }

            mated = true;  // Recognize that conjugation just happened
            mate.setConjugated();  // Tell partner that they just conjugated  

            refreshBacteria();  // Refresh self
            mate.refreshBacteria();  // Refresh mate


        }

        void duplicate(vector<bacteria>& germs)
        {
            /* This launches a duplication attempt.
            */
        


            if (germs.size() == MAX - 1)  // Enforce a hard cap
                return;

            if (stomach < 0)  // Quick fix to population boom issue. Real cause unknown
            {
                dead = true;
                return;
            }

            // Variables
            bacteria offspring;
            sf::Vector2f whereami = bug.getPosition();
            int mutagenVal = 0;

            // Abort if unable to duplicate or try to copy genes if it can
            if (stomach <= genome.at(1) || genome.at(3) < 1 || dead)
                return;
            
            else if (offspringNum > genome.at(3))  // If max offspring had
                return;
            
            else
            {
                for (int x = 0; x < genome.size(); x++)
                {
                    mutagenVal = randInt(-5, 5);

                    if (randInt(1, 100) < 98)  // If no mutation
                        offspring.setGene(x, genome.at(x));  // Set the offspring's gene to be equal to own
                    
                    else
                        offspring.setGene(x, genome.at(x) + mutagenVal);  // Mutate the gene 
                }

            }


            // Determine if dublication failed
            if (offspring.genome.at(1) < 0 || offspring.genome.at(2) < 0)  // If negative speed, size, or strength
                return;  // The duplication fails
            
            if (offspring.genome.at(9) < 0)  
                offspring.setGene(9, 0);

            if (offspring.genome.at(9) > 3)
                offspring.setGene(9, 3); 

            if (offspring.genome.at(0) < 0)
                offspring.setGene(0, 0);

            // Give birth
            offspring.refreshBacteria();  // Solidify new genome
            offspring.move(whereami.x, whereami.y);  // Give birth where you're at
            germs.push_back(offspring);  // Introduce to population  ; THIS CAUSES SEG FAULT IF UNLIMITED
            //cout << "Bug born to parent of " << offspringNum << "\n";
            offspringNum++;
            born++;
            stomach -= genome.at(1);
            
        }

        
        // System ============
        void draw(sf::RenderWindow &window){
            if (!dead)  // Only draw living bugs
            {
                window.draw(bug);
            }
            
        }


    private:
        sf::CircleShape bug;  // This is the entity

        // Information
        int memX = 0;  // This is a memory of the last x movement
        int memY = 0;  // This is a memory of the last y movement
        int lastDistance = 999999;  // Having just been born, the bacteria is far from food
        bool closerToFood = false;  // If this is true, change by memory
        bool seesFood = false;
        bool mated = false;
        int offspringNum = 0;
        int radius = 1;

        // Body
        bool dead = false;
        int stomach = 0;  // For every food collected, this ++; need to == size to have proliferation chance
        int age = 0; 
        vector<int> genome{1, 1, 1, 1, 5000, 1, 1, 1, 5000, 0};  // [Speed, size, strength, fertility, sight, color1, color2, color3, lifespan, trophism]
};

class food{
    public:
        food()
        {
            chum.setRadius(static_cast<float>(radius));
            chum.setFillColor(sf::Color::Green);
        }

        int getRadius()
        {
            return radius;
        }

        void move(int x, int y)
        {
            chum.setPosition(x, y);
        }

        sf::Vector2f pos()
        { 
            /* Gives position of food as (x, y)*/
            return chum.getPosition();
        }

        int eat()
        {
            eaten = true;
            return calories;
        }

        bool isEaten()
        {
            return eaten;
        }

        void setEaten()
        {
            eaten = true;
        }

        void draw(sf::RenderWindow &window)
        {
            if (!eaten)  // Only render if not eaten
            {
                window.draw(chum);
            }
            
        }

    private:
        sf::CircleShape chum;
        int radius = 1;
        bool eaten = false;
        int calories = 1;


};


// ======= FUNCTIONS

int randInt(int lower, int upper)
{
    random_device rd;
    default_random_engine eng(rd());
    uniform_int_distribution<int> distr(lower, upper);
    return distr(eng);
}

void moveBacteria(bacteria &bug)
{
    sf::Vector2f pos = bug.pos();
    int changex = pos.x, changey = pos.y;  // Assign change to original value
    
    // Read keyboard for input
    // If no inout, just use default change (implied)

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        changex = pos.x - 1;
        changey = pos.y -1;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        changex = pos.x + 1;
        changey = pos.y - 1;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        changex = pos.x - 1;
        changey = pos.y + 1;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        changex = pos.x + 1;
        changey = pos.y + 1;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
        changex = pos.x;
        changey = pos.y - 1;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
        changex = pos.x - 1;
        changey = pos.y;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
        changex = pos.x + 1;
        changey = pos.y;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
        changex = pos.x;
        changey = pos.y + 1;
    }


    // Set border. Prevent entity from escaping screen
    if (changex > 800){
        changex = 800;
    }
    
    if (changex < 0){
        changex = 0;
    }

    if (changey > 600){
        changey = 600;
    }
    
    if (changey < 0){
        changey = 0;
    }

    bug.move(changex, changey);
}

// ====== Simulation Functions
void moveGerms(vector<bacteria>& bugs)
{
    // Variables 
    int population = bugs.size();

    // Move them accordingly
     
    for (int x = 0; x < population; x++)
    {
        if (bugs.at(x).isCloserToFood())
            bugs.at(x).redoLastMove();
        
        else
        {
            bugs.at(x).randomWalk();
        }
            
    }

}

int inVector(string query, vector<string> genomes)
{
    /* 
    This will scan a vector for a string. If found, will relay its position.
    Otherwise, it'll say -404
    */

    // Variables
    int size = genomes.size();

    // Scan
    if (size == 0)
        return -404;  // Dont scan empty vector

    for (int x = 0; x < size; x++)
    {
        if (query == genomes.at(x))
        return x;
    }

    return -404; // This gets sent if it wasn't found
}

void poll(vector<bacteria>& germs)
{
    /*
        Iterates through population, reads genomes, then outputs them to a text file 
        with a number corresponding to its prevalence. This may be a bit beefy, so 
        it should not be ran frequently.
    */

    // Variables
    vector<string> genomes;  // Holds the string form of the genome
    vector<int> counts;   // Holds the incidence count for a particular string form
    int population = germs.size(); // Size of the population
    string tempGenome;  // This will temporarily hold a genome
    int recordSize = 0;  // This will be updated later to reflect the known genomes
    int position = -10;  // Placeholder value for position 

    // 1. Iterate through the genome and get info
    // The idea is that as genomes grows, so does counts, so that any genome at position x will have a count at position x
    for (int x = 0; x < population; x++)
    {
        tempGenome = germs.at(x).getGenomeString();
        position = inVector(tempGenome, genomes);

        if (position == -404)  // If we havent seen this genome before
        {
            genomes.push_back(tempGenome);
            counts.push_back(1);  // Record that we've now seen it once
        }
        else  // If we have seen it before
        {
            counts.at(position)++;  // Iterate the count for this genome by 1
        }
    }


    // 2. Output data to file 
    recordSize = genomes.size();
    ofstream file("recordBook.txt"); // Open output file

    for (int x = 0; x < recordSize; x++)
        file << genomes.at(x) << ": " << to_string(counts.at(x)) << "\n";

    file.close(); 
    


}


void duplicate(vector<bacteria>& germs)
{
    /* This should iterate over original population, seeing if they 
    can duplicate. If so, the offspring will be added to the back and
    wont be iterated over until the next frame.
    

    */

    int size = germs.size();
    bool finished = false;
    int place = 0;

    for (int x = 0; x < size; x++)
    { 
        germs.at(x).duplicate(germs);  // This will asses if they can duplicate and push offspring to back.
    }
}


void prune(vector<bacteria>& bugs, vector<food>& sugars)
{
    
    // Variables
    bool clean = false;  // While corpses or food-memories exist in vector, this is false
    int size = bugs.size();
    bacteria temp;
    food foodTemp;
    
    // Clean corpses
    while (!clean)
    {
        size = bugs.size();
        for (int x = 0; x < size;x++)
        {
            if (bugs.at(x).isDead() && x != size - 1)  // If we've found a dead one and its not the last element 
            {
                // Swap with last element, then delete
                temp = bugs.at(size - 1);  // Save last element 
                bugs.at(size - 1) = bugs.at(x);  // Replace last element with current element
                bugs.at(x) = temp;  // Replace current element with last element
                bugs.pop_back();  // Delete the last element, which is our current element 
                break;
            }
            else if (bugs.at(x).isDead() && x == size - 1 && size > 1)  // If the last element is dead and the vector size is greater than 1
            {
                bugs.pop_back();  // Delete the last element
                break;
            }
            else if (bugs.at(x).isDead() && size == 1)  // If the only bug in vector is dead
            {
                clean = true;  // Set clean without doing anything
                break; 
            }
            else if (x == size - 1)  // If we got through the entire list without seeing a corpse
            {
                clean = true;  // Set clean and exit
                break;
            }
        }

    }

    // Forget meals of days past
    clean = false;

    while (!clean)
    {
        size = sugars.size();

        for (int x = 0; x < size; x++)
        {
            if (sugars.at(x).isEaten() && x != size - 1)
            {
                // Swap with last element, then delete
                foodTemp = sugars.at(size - 1);  // Save last element 
                sugars.at(size - 1) = sugars.at(x);  // Replace last element with current element
                sugars.at(x) = foodTemp;  // Replace current element with last element
                sugars.pop_back();  // Delete the last element, which is our current element 
                break; 
            }
            else if (sugars.at(x).isEaten() && x == size - 1 && size > 1)  // If the last element is eaten and the vector size is greater than 1
            {
                sugars.pop_back();  // Delete the last element
                break;
            }
            else if (sugars.at(x).isEaten() && size == 1)  // If the only sugar in vector is dead
            {
                clean = true;  // Set clean without doing anything
                break; 
            }
            else if (x == size - 1)  // If we got through the entire list without seeing a corpse
            {
                clean = true;  // Set clean and exit
                break;
            }
            
        }
    }


}

int getDistance(sf::Vector2f self, sf::Vector2f other, int selfRadius, int otherRadius)
{
    // This uses the new modified distance formula
    return (abs(other.x - self.x) + abs(other.y - self.y)) - (selfRadius + otherRadius);
}

void bugEatBug(bacteria& self, bacteria& prey)
{
    // Variables
    int mySize = self.getGene(1);
    int preySize = prey.getGene(1);
    int preyTrophism = prey.getGene(9);
    int chance = randInt(0, 1);
    int myStrength = self.getGene(2);
    int preyStrength = prey.getGene(2);

    // Determine outcome of eat attempt
    if (mySize > preySize)  // If I am bigger than prey, kill
    {
        self.fillStomach(preySize);
        prey.kill("a larger predator");
    }

    else if (mySize == preySize)  // Else if we're equal, try
    {
        if (preyStrength < myStrength)  // I won
        {
            self.fillStomach(preySize);
            prey.kill("a larger predator");
        }
        else if (preyStrength > myStrength && preyTrophism > 0)   // I lost and they can eat me
        {
            prey.fillStomach(mySize);
            self.kill("a stronger prey");
        }
        else if (preyStrength == myStrength && preyTrophism > 0)  // We're equal, its 50-50
        {
            if (chance == 1)
            {
                self.fillStomach(preySize);
                prey.kill("an equally sized predator");
            }
            else
            {
                prey.fillStomach(mySize);
                self.kill("an equally sized prey");
            }
        }
    }

}

void eatConjugateSee(vector<bacteria>& bugs)
{
    /*
    This function lets the bugs look at every other bug and food. Several 
    comparisions will be made, the most important of which is distance.

    
    */


    
}

void conjugate(bacteria self, bacteria other)
{
    // donor gives dna, recipient takes it and incorperates 

}

void see(bacteria self, bacteria other)
{

}

void getSugar(vector<bacteria>& bugs, vector<food>& sugars)
{
    /*
    This function has the bugs eat sugar.
    Causes core dump. 
    terminate called after throwing an instance of 'std::out_of_range'
  what():  vector::_M_range_check: __n (which is 198) >= this->size() (which is 198)
Aborted (core dumped)

    */
   // Variables
   int bugSize = bugs.size();
   int foodSize = sugars.size();
   int distance = 999999;
   int trophism = 0;
   int sight = 0;
   int change = 0;
   bool eaten = 0;

   sf::Vector2f myPos;
   sf::Vector2f foodPos;
   vector<int> myPlace;
   int foodPlace[] = {0, 0};
   int projectedDist = 999999;

   // Begin comparison 
   for (int x = 0; x < bugSize; x++)  // For every element in the vector
   {
    if (bugs.at(x).isDead())
        continue;

    trophism = bugs.at(x).getGene(9);  // What the current bug can eat
    sight = bugs.at(x).getGene(4);  // The distance the bug can see
    myPos = bugs.at(x).pos();  // Where the bug is in the map
    myPlace = bugs.at(x).getMemory();  // Remember where you last were


    for (int j = 0; j < foodSize; j++)  // For every sugar
    {
        foodPos = sugars.at(j).pos();
        eaten = sugars.at(j).isEaten();

        // 1. Calc distance
        distance = getDistance(myPos, foodPos, bugs.at(x).getRadius(), sugars.at(j).getRadius());

        // 2. Determine if it can be eaten
        if (trophism == 0 || trophism == 3 && !eaten)  // If it can be eaten by the current bug
        {
            if (distance <= bugs.at(x).getGene(1))  // If the food is ontop of the bug 
            {
                bugs.at(x).fillStomach(sugars.at(j).eat());  // Eat the food
                //cout << "Bug at " << myPos.x << "," << myPos.y << " has eaten a food at " << foodPos.x << "," << foodPos.y 
                //     << ". It was " << bugs.at(x).getGene(1)  << " points large and " << distance << " far away. EATEN " << eaten << "\n";
                break;  // Having eaten, this bug will be sated for the turn
            }
            else if (distance <= sight  && !eaten)  // Otherwise, if the bug can at least see it
            {
                // Determine if moving the same way will get it to where it needs to go
                // If the last memory applied to current position makes bug closer, things are getting better 
                // Otherwise, random walk 

                // 1. Figure out if applying last change is ideal
                myPlace.at(0) += myPos.x;
                myPlace.at(1) += myPos.y;
                projectedDist = abs(((foodPos.y - myPlace.at(1)) * 2) + ((foodPos.x - myPlace.at(0)) * 2));

                if (projectedDist < distance)  // If we got closer
                {
                    bugs.at(x).setCloser(true);  // Set that we have gotten closer
                }
                else
                {
                    bugs.at(x).setCloser(false);  // Set that we have not gotten closer
                }


            }
            


        }


    }

   }


}

int getPopulation(vector<bacteria>& bugs)
{
    int pop = 0;
    for (int x = 0; x < bugs.size(); x++)
    {
        if (!bugs.at(x).isDead())
            pop++;
    }
    return pop;
}

void killBugs(vector<bacteria>& bugs)
{
    /*
    This function will go through the bugs and determine if they must die
    */

    // Variables
    int size = bugs.size();

    // Look through bugs and slaughter PRN
    for (int x = 0; x < size; x++)
    {
        bugs.at(x).isTooOld();
    }
}

void investigate(vector<bacteria>& bugs, int worldAge)
{
    int size = bugs.size();
    ofstream file("report.txt");

    file << "===WORLD===\nAge = " << worldAge << "\nBorn = " << born << "\nSim Time = " << SIMFOR << "\n===POPULATION STATS===\n";

    for (int x = 0; x < size; x++)
    {
        file << "Bug " << x << ":AGE= " << bugs.at(x).getAge()  << ";STOMACH=" << bugs.at(x).getStomach() << ";DEAD=" << bugs.at(x).isDead()
             << ";GENOME=" << bugs.at(x).getGenomeString() << ";OFFSPRING=" << bugs.at(x).getOffspringCount()  
             << ";HASCONJUGATED=" << bugs.at(x).hasConjugated() << "\n";

    }
    file.close();
}

void resetSugars(vector<food>& sugars)
{
    int size = sugars.size();
    vector<sf::Vector2f> positions;
    int numPos = positions.size();
    int xPos = 0;
    int yPos = 0;
    bool newPlace = false;
    food newSugar;

    // 1. Learn where the sugar is at
    for (int x = 0; x < size; x++)
    {
        if (!sugars.at(x).isEaten())  // If the sugar hasn't been eaten, record its place
            positions.push_back(sugars.at(x).pos());  // Add this position to known placements
    }
    numPos = positions.size();

    // 2. Randomly place eaten ones
    for (int x = 0; x < MAX_FOOD - size; x++)  // For each sugar
    { 
        do
        {
            xPos = randInt(0, LENGTH);
            yPos = randInt(0, WIDTH);

            for (int j = 0; j < numPos; j++)
            {
                if (positions.at(j).x == xPos && positions.at(j).y == yPos)
                {
                    newPlace = false;
                    break;
                }
                else if (j == numPos - 1)
                {
                    newPlace = true;
                }
            }

        } while (!newPlace);

        newSugar.move(yPos, xPos);
        sugars.push_back(newSugar);        
    }
}

void compareBugs(bacteria& self, bacteria& other)
{
    // This will do comparisons for two bugs, testing if they meet criteria for 
    // predation or conjugation

    // Variables
    int distance = getDistance(self.pos(), other.pos(), self.getRadius(), other.getRadius());  // This is the absolute distance using modifed dist formula
    
    int selfSize = self.getGene(1);
    int selfStrength = self.getGene(2);
    int selfTroph = self.getGene(9);
    string selfGenome = self.getGenomeString();
    bool selfDead = self.isDead();
    bool selfConjugated = self.hasConjugated();


    int otherSize = other.getGene(1);
    int otherStrength = other.getGene(2);
    int otherTroph = other.getGene(9);
    string otherGenome = other.getGenomeString();
    bool otherDead = other.isDead();
    bool otherConjugated = other.hasConjugated();

    // 1. Determine if valid comparison 
    if (selfDead || otherDead || (distance > selfSize && distance > otherSize))  // If either is dead or too far
        return;  // Invalid comparison, so end


    // 2. Determine if either can eat each other 

    // 2.1 Start with self
    if (selfTroph >= 2 && distance <= selfSize && selfSize >= otherSize)  // If other is edible not the same species
    {
        // Have self eat other 
    }
    else if (selfTroph == 1 && selfGenome == otherGenome && selfSize >= otherSize)  // If homotroph and the other is same species
    {
        // eat bug
    }

    // 2.2 End with other

}

void lookAtPopulation(vector<bacteria>& bugs)
{
    // This function will compare each element of population to every other element in vector that is < to i

    for (int x = 0; x < bugs.size(); x++)
    {
        for (int j = 0; j < bugs.size(); j++)
        {
            if (x > j)
                compareBugs(bugs.at(x), bugs.at(j));
        }
    }

}


// ====== Start Main
int main()
{

    // ============ STARTING CONDITIONS ============
    // Setup Window and time
    sf::RenderWindow window(sf::VideoMode(LENGTH, WIDTH), "Bacteria Sim");
    sf::Clock clock;
    sf::Clock totalTimeClock;
    sf::Time elapsedtime;
    

    
    // Create entities and variables
    const int START = 1;
    int population = START;
    int glucose = MAX_FOOD;  // Max glucose avaliable on
    int sugarSize = glucose;  // Represents the size of the vector
    bool blue = true;
    sf::Color background = sf::Color(0, 94, 184);

    int time = 0;  // This will be used to poll periodically
    int timePassed = 0;  // This will be used to asses the total amount of time passed
    int timeToPoll = 600;  // The number of frames that need to pass before a poll

    bacteria base;  // This is the luca
    bacteria temp;  // This is the temp that will be used for replication
    food sugar;  // This is the sugar base

    vector<bacteria> germs;  // This is the vector that will hold population
    vector<food> sugars;  // This is the vector that will hold food

    germs.reserve(MAX);  // Reserves memory for population growth

    for (int x = 0; x < START; x++)  // Add individuals to population
    {
        germs.push_back(base);
        germs.at(x).move(randInt(0, LENGTH), randInt(0, WIDTH));
    }

    for (int x = 0; x < glucose; x++)  // Add sugar crystals to world
    {
        sugars.push_back(sugar);
        sugars.at(x).move(randInt(0, LENGTH), randInt(0, WIDTH));  // Randomly move this food somewhere
    }

    // Setup text
    sf::Text position;  // This entity will feed info from the environment
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
	{
		// Be chill
	}

    position.setFont(font);
    position.setCharacterSize(24);
    position.setFillColor(sf::Color::White);
    position.setPosition(0, 0);


    // ============ MAIN PROGRAM ============
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))  // Shutdown if window closed
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }


        // ============ RUN SIMULATION FRAME ============

        // 0. Kill
        killBugs(germs);

        // 1. Move bugs
        moveGerms(germs);

        // 2. Let bugs eat
        getSugar(germs, sugars);
        eatConjugateSee(germs); 

        // 3. Conjugate if possible

        // 4. Give bugs info over environment

        // 5. Division 
        duplicate(germs);

        // 6. Poll if its time
        if (time >= timeToPoll)
        {
            time = 0;  // Reset the clock
            poll(germs);
        }

        // ============ OTHER FUNCTIONS + DRAWING ============

        // 7. Prune dead and eaten for memory
        prune(germs, sugars);

        // Move Jack and report on his life
        elapsedtime = totalTimeClock.getElapsedTime();
        position.setString("Population: " + to_string(getPopulation(germs)) + "\nFood: " + to_string(sugars.size()) + "\nTurn " + to_string(timePassed) 
                           + "\nTimer: " + to_string(elapsedtime.asSeconds()));

        // Clear original frame
        window.clear(background);  // This ensures an ocean blue background

        // Determine if sim must end
        population = germs.size();
        elapsedtime = totalTimeClock.getElapsedTime();
        if (elapsedtime.asSeconds() >= SIMFOR && elapsedtime.asSeconds() > 0)  // If the simulation has run for the specified amount of time and is not negative
        {
            investigate(germs, timePassed);  // Harvest data
            cout << "Simulation ended at " << timePassed << ". " << born << " entities were created.\n";
            return 1;
        }

        // Draw germ population
        if (population == 1 && germs.at(0).isDead())  // If there's only one germ left in vector and it's dead, dont draw
        {
            population = 0;
            investigate(germs, timePassed);  // Harvest data
            cout << "Apocalypse occurred at turn " << timePassed << ". " << born << " bacteria were born." << "\n";
            break;
        }
        else if (population == 0)
        {
            investigate(germs, timePassed);  // Harvest data
            cout << "Apocalypse occurred at turn " << timePassed << "\n";
            break;
        }
        else if (RENDER_BUGS) // Otherwise, draw as usual 
        {
            for (int x = 0; x < population;x++)  // Draw bugs
            {
                germs.at(x).draw(window);
            }
        }

        // Draw & replenish sugars
        elapsedtime = clock.getElapsedTime();
        if (elapsedtime.asSeconds() > REFIL_SEC)
        {
            resetSugars(sugars);
            clock.restart();
        }


        sugarSize = sugars.size();  
        if (sugarSize > 1)  // If the food is sufficient, draw
        {
            for (int x = 0; x < sugars.size(); x++)  // Draw food
            {
                sugars.at(x).draw(window);
            }
        }

        window.draw(position);
        window.display();
        time++;
        timePassed++;
    }

    return 0;
}

