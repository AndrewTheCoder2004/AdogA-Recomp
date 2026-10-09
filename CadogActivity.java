package com.cadog.game;

import org.libsdl.app.SDLActivity;

public class CadogActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "cadog"
        };
    }
}
