#ifndef AL_SPOUT_H
#define AL_SPOUT_H

#include "al_console.h"
#include "al_glm.h"

#include "spout2/SpoutLibrary.h"


struct StreamTexture {
    SPOUTHANDLE spout = 0;
    glm::ivec2 dim;

    bool init(glm::ivec2 texdim) {
        
        if (!spout) spout = GetSpout();

        if (!spout) {
            console.error("failed to create Spout library");
            return false;
        } else {
            dim = texdim;
            spout->CreateSender("Alice", dim.x, dim.y);
            console.log("ready to spout at %d x %d", dim.x, dim.y);
        }
    }

    ~StreamTexture() {
        spout->ReleaseSender();
    }

    void send(GLuint id) {
        if (spout) {
			spout->SendTexture(id, GL_TEXTURE_2D, dim.x, dim.y);
		}
    }

};

#endif