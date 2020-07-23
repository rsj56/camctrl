#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <iterator>

#include "camera_utils.h"


void CameraUtils::start_preview(void) {
    std::string cmd = "raspistill " + get_preview_arg() +
            " -t 0 -s -o " + projectDir + "temp.jpg &";
    system(cmd.c_str());
};

void CameraUtils::stop_preview(void) {
    std::string cmd = "killall raspistill &&";
    system(cmd.c_str());
};

void CameraUtils::start_recording(void) {

};

void CameraUtils::stop_recording(void) {

};

void CameraUtils::capture(void) {
    std::string cmd;
    cmd = "pgrep raspistill | xargs -I % kill -USR1 % &&";
    system(cmd.c_str());
    cmd = "cd " + projectDir;
    system(cmd.c_str());
    cmd = "ls -1 | grep IMG | wc -l | xargs printf \"%04d\" | xargs -I % mv temp.jpg IMG_%.jpg";
    system(cmd.c_str());
};

void CameraUtils::set_projectDir(QString dir) {
    projectDir = dir.toStdString();
};



// Private

std::string CameraUtils::get_preview_arg(void) {
    std::string arg = "-p '" +
            std::to_string(CAMERA_VIEWPORT_X) + "," +
            std::to_string(CAMERA_VIEWPORT_Y) + "," +
            std::to_string(CAMERA_VIEWPORT_WIDTH) + "," +
            std::to_string(CAMERA_VIEWPORT_HEIGHT) + "'";
    return arg;
};

