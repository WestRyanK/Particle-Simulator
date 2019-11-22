#ifndef _ICONTROLLABLE_H_
#define _ICONTROLLABLE_H_

#include <string>
using namespace std;

namespace CodeMonkeys::Engine::Control
{
    class IControllable
    {
    public:
        // virtual void control(std::string control_name, float value_a, float value_b) = 0;
        virtual void control(std::string control_name, float value, float dt) = 0;
        virtual void control(std::string control_name, int value, float dt) = 0;
    };
}

#endif