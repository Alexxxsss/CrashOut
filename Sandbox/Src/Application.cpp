#include "Application.h" 
#include "Engine.h" 
#include <iostream>

void OnStart()
{
    std::cout << "Game is starting!" << std::endl;
}

// Här anropar du motorns renderingsfunktioner
// Här läser du input och flyttar spelfigurer via motorns API
void OnUpdate()
{
    
}

void OnRenderFrame()
{

}

// Här rensar du spelspecifikt minne om det behövs
void OnCleanUp()
{
    std::cout << "Game is quiting and freeing upp memory" << std::endl;
}