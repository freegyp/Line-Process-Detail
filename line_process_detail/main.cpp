
//
// Disclamer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resource, use the helper
// method resourcePath() from ResourcePath.hpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <iostream>
#include <cmath>
#include <fstream>

// Here is a small helper for you ! Have a look.
#include "ResourcePath.hpp"
using namespace std;

bool negligible(sf::Vector2f left,sf::Vector2f right){
    sf::Vector2f diff=left-right;
    if (diff.x<1e-4&&diff.x>-1e-4) {
        if (diff.y<1e-4&&diff.y>-1e-4) {
            return true;
        }
    }
    return false;
}

struct MSTNode{
    int ID;                             //Marked uniquely based on its exact location on the screen.
    sf::Vector2f position;
    float traveled_distance;
    float local_radius;
    vector<MSTNode*> neighbours;
    int num_of_neighbours;
    MSTNode(sf::Vector2f pos,int id){
        position=pos;
        ID=id;
        num_of_neighbours=0;
    }
    bool isInTree(int id,set<int>* gone){
        gone->insert(ID);
        for(int i=0;i<num_of_neighbours;i++){
            if(gone->count(neighbours[i]->ID)==0){
                if(neighbours[i]->ID==id)return true;
                if(neighbours[i]->isInTree(id, gone))return true;
            }
        }
        return false;
    }
    bool addNeighbour(MSTNode* neighbour){
        if(num_of_neighbours!=0)
            for(int i=0;i<num_of_neighbours;i++){
                if(neighbours[i]==neighbour){
                    return false;
                }
            }
        neighbours.push_back(neighbour);
        num_of_neighbours++;
        neighbour->addNeighbour(this);
        return true;
    }
    void drawFromThisNode(sf::RenderWindow* window,set<int>* gone){
        gone->insert(ID);
        for(int i=0;i<num_of_neighbours;i++){
            if(gone->find(neighbours[i]->ID)==gone->end()){
                sf::VertexArray line;
                line.append(sf::Vertex(position,sf::Color::Black));
                line.append(sf::Vertex(neighbours[i]->position,sf::Color::Black));
                line.setPrimitiveType(sf::PrimitiveType::Lines);
                window->draw(line);
                neighbours[i]->drawFromThisNode(window,gone);
            }
        }
    }
};

struct movPoint{
    sf::Vector2f curPos;
    sf::Vector2f gradient;
    float distance_traveled=0;
    bool hasStopped=false;
    void checkState(multimap<int,movPoint*>* pixelMap,int imgX,int imgY){
        if(hasStopped)return;
        vector<movPoint*> toCompare;
        multimap<int,movPoint*>::iterator it;
        int center=((int)curPos.y)*imgX+(int)curPos.x;
        for(it=pixelMap->equal_range(center).first;it!=pixelMap->equal_range(center).second;it++){
            sf::Vector2f thatPos=(*it).second->curPos;
            sf::Vector2f diff=thatPos-curPos;
            if(diff.x*diff.x+diff.y*diff.y<=1){
                toCompare.push_back((*it).second);
            }
        }
        int upleft=((int)curPos.y-1)*imgX+(int)curPos.x-1;
        for(it=pixelMap->equal_range(upleft).first;it!=pixelMap->equal_range(upleft).second;it++){
            sf::Vector2f thatPos=(*it).second->curPos;
            sf::Vector2f diff=thatPos-curPos;
            if(diff.x*diff.x+diff.y*diff.y<=1){
                toCompare.push_back((*it).second);
            }
        }
        int up=((int)curPos.y-1)*imgX+(int)curPos.x;
        for(it=pixelMap->equal_range(up).first;it!=pixelMap->equal_range(up).second;it++){
            sf::Vector2f thatPos=(*it).second->curPos;
            sf::Vector2f diff=thatPos-curPos;
            if(diff.x*diff.x+diff.y*diff.y<=1){
                toCompare.push_back((*it).second);
            }
        }
        int upright=((int)curPos.y-1)*imgX+(int)curPos.x+1;
        for(it=pixelMap->equal_range(upright).first;it!=pixelMap->equal_range(upright).second;it++){
            sf::Vector2f thatPos=(*it).second->curPos;
            sf::Vector2f diff=thatPos-curPos;
            if(diff.x*diff.x+diff.y*diff.y<=1){
                toCompare.push_back((*it).second);
            }
        }
        int left=((int)curPos.y)*imgX+(int)curPos.x-1;
        for(it=pixelMap->equal_range(left).first;it!=pixelMap->equal_range(left).second;it++){
            sf::Vector2f thatPos=(*it).second->curPos;
            sf::Vector2f diff=thatPos-curPos;
            if(diff.x*diff.x+diff.y*diff.y<=1){
                toCompare.push_back((*it).second);
            }
        }
        int right=((int)curPos.y)*imgX+(int)curPos.x+1;
        for(it=pixelMap->equal_range(right).first;it!=pixelMap->equal_range(right).second;it++){
            sf::Vector2f thatPos=(*it).second->curPos;
            sf::Vector2f diff=thatPos-curPos;
            if(diff.x*diff.x+diff.y*diff.y<=1){
                toCompare.push_back((*it).second);
            }
        }
        int lowerleft=((int)curPos.y+1)*imgX+(int)curPos.x-1;
        for(it=pixelMap->equal_range(lowerleft).first;it!=pixelMap->equal_range(lowerleft).second;it++){
            sf::Vector2f thatPos=(*it).second->curPos;
            sf::Vector2f diff=thatPos-curPos;
            if(diff.x*diff.x+diff.y*diff.y<=1){
                toCompare.push_back((*it).second);
            }
        }
        int lower=((int)curPos.y+1)*imgX+(int)curPos.x;
        for(it=pixelMap->equal_range(lower).first;it!=pixelMap->equal_range(lower).second;it++){
            sf::Vector2f thatPos=(*it).second->curPos;
            sf::Vector2f diff=thatPos-curPos;
            if(diff.x*diff.x+diff.y*diff.y<=1){
                toCompare.push_back((*it).second);
            }
        }
        int lowerright=((int)curPos.y+1)*imgX+(int)curPos.x+1;
        for(it=pixelMap->equal_range(lowerright).first;it!=pixelMap->equal_range(lowerright).second;it++){
            sf::Vector2f thatPos=(*it).second->curPos;
            sf::Vector2f diff=thatPos-curPos;
            if(diff.x*diff.x+diff.y*diff.y<=1){
                toCompare.push_back((*it).second);
            }
        }
        for(vector<movPoint*>::iterator it2=toCompare.begin();it2!=toCompare.end();it2++){
            sf::Vector2f thatGrad=(*it2)->gradient;
            if(gradient.x*thatGrad.x+gradient.y*thatGrad.y<0){
                hasStopped=true;
                return;
            }
        }
    }
    bool move(vector<vector<sf::Vector2f> >* gradientTable){
        float xPos=curPos.x/2;
        float yPos=curPos.y/2;
        if(xPos<=gradientTable->size()&&xPos>=0){
            if(yPos<=(*gradientTable)[0].size()&&yPos>=0){
                int xIndex=xPos;
                int yIndex=yPos;
                sf::Vector2f passingGrad=(*gradientTable)[xIndex][yIndex];
                if(gradient.x*passingGrad.x+gradient.y*passingGrad.y>0){
                    curPos+=0.00001f*gradient;
                    distance_traveled+=0.00001*sqrt(gradient.x*gradient.x+gradient.y*gradient.y);
                    return true;
                }
            }
        }
        return false;
    }
};

int main(int, char const**)
{
    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return EXIT_FAILURE;
    }
    

    sf::Image image;
    if (!image.loadFromFile(resourcePath()+"img-7.png")) {
        return EXIT_FAILURE;
    }

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(image.getSize().x*2, image.getSize().y*2), "Clustering");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    float maxGradient=0;
    vector<vector<sf::Vector2f> > gradients;
    for (int i=0; i<image.getSize().x-1; i++) {
        vector<sf::Vector2f> gradientLine;
        for (int j=0; j<image.getSize().y-1; j++) {
            sf::Color selfColor=image.getPixel(i, j);
            float selfIntensity=selfColor.r*selfColor.r+selfColor.g*selfColor.g+selfColor.b*selfColor.b+selfColor.a*selfColor.a;
            selfIntensity=sqrt(selfIntensity);
            sf::Color xNext=image.getPixel(i+1, j);
            float xNextIntensity=xNext.r*xNext.r+xNext.g*xNext.g+xNext.b*xNext.b+xNext.a*xNext.a;
            xNextIntensity=sqrt(xNextIntensity);
            sf::Color yNext=image.getPixel(i, j+1);
            float yNextIntensity=yNext.r*yNext.r+yNext.g*yNext.g+yNext.b*yNext.b+yNext.a*yNext.a;
            yNextIntensity=sqrt(yNextIntensity);
            sf::Vector2f gradient(-xNextIntensity+selfIntensity,-yNextIntensity+selfIntensity);
            if (sqrt(gradient.x*gradient.x+gradient.y*gradient.y)>maxGradient) {
                maxGradient=sqrt(gradient.x*gradient.x+gradient.y*gradient.y);
            }
            gradientLine.push_back(gradient);
        }
        gradientLine.push_back(sf::Vector2f(0,0));
        gradients.push_back(gradientLine);
    }
    vector<sf::Vector2f> lastLine;
    for (int i=0; i<image.getSize().y; i++) {
        lastLine.push_back(sf::Vector2f(0,0));
    }
    gradients.push_back(lastLine);
    
    vector<movPoint*> movingPoints;
    
    for (int i=0; i<image.getSize().x; i++) {
        for (int j=0; j<image.getSize().y; j++) {
            float thisGrad=gradients[i][j].x*gradients[i][j].x+gradients[i][j].y*gradients[i][j].y;
            thisGrad=sqrt(thisGrad);
            if (thisGrad>0.1*maxGradient) {
                movPoint* p=new movPoint;
                p->curPos=sf::Vector2f(i*2,j*2);
                p->gradient=gradients[i][j];
                movingPoints.push_back(p);
            }
            else if(thisGrad<=0.13*maxGradient){
                //gradients[i][j]=sf::Vector2f(0,0);
            }
        }
    }
    sf::VertexArray points;
    points.setPrimitiveType(sf::PrimitiveType::Points);
    int moving=0,stopped=0;
    bool isMoving=true;
    vector<sf::Vector2f> clusteredVertices;
    float total_distance_traveled=0;
    int zeroTravel=0;
    multimap<int,movPoint*> pixelMap;

    // Start the game loop
    while (window.isOpen())
    {
        
        // Process events
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Espace pressed : exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
        
        if (isMoving) {
            points.clear();
            for (vector<movPoint*>::iterator it=movingPoints.begin(); it!=movingPoints.end(); it++) {
                sf::Color colorPoint;
                if((*it)->move(&gradients)){
                    colorPoint=sf::Color::Red;
                    moving++;
                }
                else{
                    colorPoint=sf::Color::Black;
                    stopped++;
                    total_distance_traveled+=(*it)->distance_traveled;
                    if ((*it)->distance_traveled==0) {
                        zeroTravel++;
                    }
                    clusteredVertices.push_back((*it)->curPos);
                }
                sf::Vertex pp((*it)->curPos,colorPoint);
                points.append(pp);
            }
        
            if (moving>0) {
                isMoving=true;
            }
            else{
                isMoving=false;
                cout<<"Clustering finished."<<endl
                    <<"Altogether "<<stopped<<" points."<<endl
                    <<"There are "<<zeroTravel<<" points that didn't move at all"<<endl
                    <<"And the average distance traveled is "<<total_distance_traveled/(stopped-zeroTravel)
                    <<endl;
                sf::Image intermediate;
                intermediate.create(image.getSize().x, image.getSize().y, sf::Color::White);
                for (vector<movPoint*>::iterator it=movingPoints.begin(); it!=movingPoints.end(); it++){
                    int xIndex=(*it)->curPos.x/2;
                    int yIndex=(*it)->curPos.y/2;
                    intermediate.setPixel(xIndex, yIndex, sf::Color::Black);
                }
                if(!intermediate.saveToFile(resourcePath()+"intermediate.png"))cout<<"Failed to save intermediate file."<<endl;
                else cout<<resourcePath()<<endl;
            }
            moving=0;stopped=0;total_distance_traveled=0;zeroTravel=0;
        }
        else{
            window.close();
        }
        // Clear screen
        window.clear(sf::Color::White);

        window.draw(points);

        // Update the window
        window.display();
    }
    float maxGone=0;
    sf::RenderWindow window2(sf::VideoMode(image.getSize().x*2, image.getSize().y*2), "MST");
    window2.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    vector<MSTNode*> nodes;
    ofstream fout1(resourcePath()+"points.in",ofstream::out);
    int id=0;
    for (vector<movPoint*>::iterator it=movingPoints.begin(); it!=movingPoints.end(); it++){
        MSTNode* nodei=new MSTNode((*it)->curPos,id);
        nodei->traveled_distance=(*it)->distance_traveled;
        if ((*it)->distance_traveled>maxGone) {
            maxGone=(*it)->distance_traveled;
        }
        bool hasOverlap=false;
        for (vector<MSTNode*>::iterator it2=nodes.begin(); it2!=nodes.end(); it2++) {
            if (negligible((*it2)->position,(*it)->curPos)) {
                hasOverlap=true;
            }
        }
        if(!hasOverlap){
            id+=1;
            nodes.push_back(nodei);
            fout1<<nodei->ID<<" "<<nodei->position.x<<" "<<nodei->position.y<<" "<<nodei->traveled_distance<<endl;
        }
        else cout<<"Overlapping found once."<<endl;
        
    }
    fout1.close();
    for (vector<MSTNode*>::iterator it=nodes.begin(); it!=nodes.end(); it++) {
        float maxDist=(*it)->traveled_distance;
        for (vector<MSTNode*>::iterator it2=nodes.begin(); it2!=nodes.end(); it2++) {
            sf::Vector2f diff=(*it)->position-(*it2)->position;
            float d=sqrt(diff.x*diff.x+diff.y*diff.y);
            if (d<=2&&(*it2)->traveled_distance>maxDist) {
                maxDist=(*it2)->traveled_distance;
            }
        }
        (*it)->local_radius=maxDist;
    }
    cout<<"The maximum value of traveled distances is "<<maxGone<<endl;
    
    multimap<float,pair<int,int>> possible_edges;
    for (vector<MSTNode*>::iterator it=nodes.begin(); it!=nodes.end(); it++) {
        vector<MSTNode*>::iterator it2=it;
        it2++;
        while (it2!=nodes.end()) {
            sf::Vector2f diff=(*it)->position-(*it2)->position;
            float distance=sqrt(diff.x*diff.x+diff.y*diff.y);
            if(distance<=2*(*it)->local_radius&&distance<=2*(*it2)->local_radius)possible_edges.insert(pair<float,pair<int,int>>(distance,pair<int, int>((*it)->ID,(*it2)->ID)));
            it2++;
        }
    }
    
    map<int,MSTNode*> nodeDictionary;
    
    for (vector<MSTNode*>::iterator it=nodes.begin(); it!=nodes.end(); it++) {
        nodeDictionary.insert(pair<int,MSTNode*>((*it)->ID,(*it)));
    }
    
    set<int> gone_nodes;
    float maxLine=0;
    
    ofstream fout2(resourcePath()+"topology.in",ofstream::out);
    for (multimap<float,pair<int,int>>::iterator it=possible_edges.begin(); it!=possible_edges.end(); it++)
    {
        gone_nodes.clear();
        if (!nodeDictionary[(*it).second.first]->isInTree((*it).second.second, &gone_nodes)) {
            nodeDictionary[(*it).second.first]->addNeighbour(nodeDictionary[(*it).second.second]);
            fout2<<(*it).second.first<<" "<<(*it).second.second<<endl;
            maxLine=(*it).first;
        }
    }
    fout2.close();
    
    cout<<"And the maximum added line is "<<maxLine<<endl;
    
    sf::VertexArray leaves;
    leaves.setPrimitiveType(sf::PrimitiveType::Points);
    while (window2.isOpen()) {
        sf::Event event;
        while (window2.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed) {
                window2.close();
            }
            
            // Espace pressed : exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window2.close();
            }
        }
        
        window2.clear(sf::Color::White);
        
        gone_nodes.clear();
        
        
        nodes[0]->drawFromThisNode(&window2, &gone_nodes);
        window2.display();
    }
    
    return EXIT_SUCCESS;
}
