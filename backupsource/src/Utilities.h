#pragma once

// location of Queue Families

struct QueueFamilyIndices {
    int graphicsFamily = -1; //if -1 does not exist

    //check if queue families are valid
    bool isValid()
    {
        return graphicsFamily >= 0;
    }

};
