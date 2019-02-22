/*
    Copyright (C) 2015 Stephen Braitsch [http://braitsch.io]

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once
#include "ofxDatGuiTextInput.h"

class ofxDatGuiColorPicker : public ofxDatGuiTextInput {

    public:
    
        ofxDatGuiColorPicker(string label, ofColor color=ofColor::black) : ofxDatGuiTextInput(label, "XXXXXX")
        {
            mColor = color;
            mShowPicker = false;
            mType = ofxDatGuiType::COLOR_PICKER;
            setTheme(ofxDatGuiComponent::theme.get());
            
        // center the text input field //
            mInput.setTextInputFieldType(ofxDatGuiInputType::COLORPICKER);
//            setTextFieldInputColor();
            setInitialTextFieldInputColor();
            
        // setup the vbo that draws the main gradient //
            gPoints.push_back(ofVec2f(0, 0));
            gPoints.push_back(ofVec2f(0, 0));
            gPoints.push_back(ofVec2f(0, 0));
            gPoints.push_back(ofVec2f(0, 0));
            gPoints.push_back(ofVec2f(0, 0));
            gPoints.push_back(ofVec2f(0, 0));
            
        // center point of the gradient is 1/2 way between mColor & black //
            ofColor center = ofColor(mColor.r/2, mColor.g/2, mColor.b/2);
            
        // draw main gradient as a six point triangle fan //
            gColors.push_back(center);          // center
            gColors.push_back(ofColor::white);  // top-left
            gColors.push_back(mColor);          // top-right
            gColors.push_back(ofColor::black);  // btm-right
            gColors.push_back(ofColor::black);  // btm-left
            gColors.push_back(ofColor::white);  // top-left
            vbo.setColorData(&gColors[0], 6, GL_DYNAMIC_DRAW );
        }
    
        void setTheme(const ofxDatGuiTheme* theme)
        {
            ofxDatGuiTextInput::setTheme(theme);
            mStyle.stripe.color = theme->stripe.colorPicker;
            pickerRect = ofRectangle(0, 0, mInput.getWidth(), (mStyle.height*4 + mStyle.padding) * 3);
            rainbow.image = theme->icon.rainbow;
            rainbow.rect = ofRectangle(0, 0, theme->layout.colorPicker.rainbowWidth, pickerRect.height - (mStyle.padding * 2));
            gradientRect = ofRectangle(0, 0, pickerRect.width - rainbow.rect.width - (mStyle.padding * 3), rainbow.rect.height);
            pickerBorder = theme->color.colorPicker.border;
            setTextFieldInputColor();
        }
    
        void setColor(ofColor color)
        {
            mColor = color;
            setTextFieldInputColor();
        }
    
        void setColor(int hex)
        {
            mColor = ofColor::fromHex(hex);
            setTextFieldInputColor();
        }
    
        void setColor(int r, int g, int b, int a = 255)
        {
            mColor = ofColor(r, g, b, a);
            setTextFieldInputColor();
        }
    
        ofColor getColor()
        {
            return mColor;
        }
    
        void draw()
        {
            //TODO: Draw grey area when color picker is expanded
            if (!mVisible) return;
            ofPushStyle();
                ofxDatGuiTextInput::draw();
                if (mShowPicker) {
                    pickerRect.x = this->x + mLabel.width;
                    pickerRect.y = this->y + mStyle.padding + mInput.getHeight();
                    pickerRect.width = mInput.getWidth();
                    rainbow.rect.x = pickerRect.x + pickerRect.width - rainbow.rect.width - mStyle.padding;
                    rainbow.rect.y = pickerRect.y + mStyle.padding;
                    gradientRect.x = pickerRect.x + mStyle.padding;
                    gradientRect.y = pickerRect.y + mStyle.padding;
                    gradientRect.width = pickerRect.width - rainbow.rect.width - (mStyle.padding * 3);
                    gPoints[0] = ofVec2f(gradientRect.x+ gradientRect.width/2, gradientRect.y + gradientRect.height/2);
                    gPoints[1] = ofVec2f(gradientRect.x, gradientRect.y);
                    gPoints[2] = ofVec2f(gradientRect.x+ gradientRect.width, gradientRect.y);
                    gPoints[3] = ofVec2f(gradientRect.x+ gradientRect.width, gradientRect.y + gradientRect.height);
                    gPoints[4] = ofVec2f(gradientRect.x, gradientRect.y+gradientRect.height);
                    gPoints[5] = ofVec2f(gradientRect.x, gradientRect.y);
                    vbo.setVertexData(&gPoints[0], 6, GL_DYNAMIC_DRAW );
                    ofSetColor(pickerBorder);
                    ofDrawRectangle(pickerRect);
                    ofSetColor(ofColor::white);
                    rainbow.image->draw(rainbow.rect);
                    vbo.draw( GL_TRIANGLE_FAN, 0, 6 );
                }
            ofPopStyle();
        }
    
        void drawColorPicker()
        {
            if (mVisible && mShowPicker){
                ofPushStyle();
                    ofSetColor(pickerBorder);
                    ofDrawRectangle(pickerRect);
                    ofSetColor(ofColor::white);
                    rainbow.image->draw(rainbow.rect);
                    vbo.draw( GL_TRIANGLE_FAN, 0, 6 );
                ofPopStyle();
            }
        }
    
        bool hitTest(ofPoint m)
        {
            if (mInput.hitTest(m)){
                return true;
            }else if (mShowPicker && pickerRect.inside(m)){
                return true;
            }else{
                return false;
            }
        }
    
        static ofxDatGuiColorPicker* getInstance() { return new ofxDatGuiColorPicker("X"); }
    
    protected:
    
        int getHeight()
        {
            return mInput.getHeight() + (mShowPicker ? pickerRect.height : 0);
        }
    
        void onMouseEnter(ofPoint mouse)
        {
//            mShowPicker = true;
//            if (internalEventCallback != nullptr){
//                ofxDatGuiInternalEvent e(ofxDatGuiEventType::DROPDOWN_TOGGLED, mIndex);
//                internalEventCallback(e);
//            }
//            ofxDatGuiComponent::onFocus();
//            ofxDatGuiComponent::onMouseEnter(mouse);
        }
    
        void onMouseLeave(ofPoint mouse)
        {
//            mShowPicker = false;
//            if (internalEventCallback != nullptr){
//                ofxDatGuiInternalEvent e(ofxDatGuiEventType::DROPDOWN_TOGGLED, mIndex);
//                internalEventCallback(e);
//            }
//            ofxDatGuiTextInput::onMouseLeave(mouse);
//            if (!mInput.hasFocus()) ofxDatGuiComponent::onFocusLost();
        }
    
        void onMousePress(ofPoint mouse)
        {
//            ofxDatGuiComponent::onMousePress(mouse);
//            if (hitTest(mouse)){
//                unsigned char p[3];
//                int y = (mouse.y-ofGetHeight())*-1;
//                glReadPixels(mouse.x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &p);
//                gColor.r = int(p[0]);
//                gColor.g = int(p[1]);
//                gColor.b = int(p[2]);
//                if (rainbow.rect.inside(mouse))
//                {
//                    rainbowClicked = true;
//                    gradientRectClicked = false;
//                    gColors[2] = gColor;
//                    gColors[0] = ofColor(gColor.r/2, gColor.g/2, gColor.b/2);
//                    vbo.setColorData(&gColors[0], 6, GL_DYNAMIC_DRAW );
//                }
//                else if (gradientRect.inside(mouse))
//                {
//                    gradientRectClicked = true;
//                    rainbowClicked = false;
//                    mColor = gColor;
//                    // dispatch event out to main application //
//                    if (colorPickerEventCallback != nullptr) {
//                        ofxDatGuiColorPickerEvent e(this, mColor);
//                        colorPickerEventCallback(e);
//                    }   else{
//                        ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
//                    }
//                    setTextFieldInputColor();
//                }
//            }
//            if (mInput.hitTest(mouse)) mInput.onFocus();
        }
    
        void onMouseRelease(ofPoint mouse)
        {
            rainbowClicked = false;
            gradientRectClicked = false;
        }
    
        void onMouseDrag(ofPoint mouse)
        {
//            unsigned char p[3];
//            if (rainbowClicked)
//            {
//                int y = (ofClamp(mouse.y, rainbow.rect.getY()+1, rainbow.rect.getY() + rainbow.rect.getHeight())-ofGetHeight())*-1;
//                int x = rainbow.rect.getX() + rainbow.rect.getWidth()/2;
//                glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &p);
//                gColor.r = int(p[0]);
//                gColor.g = int(p[1]);
//                gColor.b = int(p[2]);
//
//                gColors[2] = gColor;
//                gColors[0] = ofColor(gColor.r/2, gColor.g/2, gColor.b/2);
//                vbo.setColorData(&gColors[0], 6, GL_DYNAMIC_DRAW );
//            }
//            else if (gradientRectClicked)
//            {
//                int y = (ofClamp(mouse.y, gradientRect.getY()+1, gradientRect.getY() + gradientRect.getHeight())-ofGetHeight())*-1;
//                int x = ofClamp(mouse.x, gradientRect.getX()+2, gradientRect.getX() + gradientRect.getWidth()-1);
//                glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &p);
//                gColor.r = int(p[0]);
//                gColor.g = int(p[1]);
//                gColor.b = int(p[2]);
//                mColor = gColor;
//                // dispatch event out to main application //
//                if (colorPickerEventCallback != nullptr) {
//                    ofxDatGuiColorPickerEvent e(this, mColor);
//                    colorPickerEventCallback(e);
//                }   else{
//                    ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
//                }
//                setTextFieldInputColor();
//            }
        }

    
        void onInputChanged(ofxDatGuiInternalEvent e)
        {
            mColor = ofColor::fromHex(ofHexToInt(mInput.getText()));
        // set the input field text & background colors //
            mInput.setBackgroundColor(mColor);
            mInput.setTextInactiveColor(mColor.getBrightness() < BRIGHTNESS_THRESHOLD ? ofColor::white : ofColor::black);
        // update the gradient picker //
            gColors[2] = mColor;
            gColors[0] = ofColor(mColor.r/2, mColor.g/2, mColor.b/2);
            vbo.setColorData(&gColors[0], 6, GL_DYNAMIC_DRAW );
        // dispatch event out to main application //
            if (colorPickerEventCallback != nullptr) {
                ofxDatGuiColorPickerEvent evt(this, mColor);
                colorPickerEventCallback(evt);
            }   else{
                ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
            }
        }

        inline void setTextFieldInputColor()
        {
        // convert color value to a six character hex string //
            std::stringstream ss;
            ss<< std::hex << mColor.getHex();
            std::string res ( ss.str() );
            while(res.size() < 6) res+="0";
            mInput.setText(ofToUpper(res));
            mInput.setBackgroundColor(mColor);
            mInput.setTextInactiveColor(mColor.getBrightness() < BRIGHTNESS_THRESHOLD ? ofColor::white : ofColor::black);
        }
    
    inline void setInitialTextFieldInputColor()
    {
        // convert color value to a six character hex string //
        std::stringstream ss;
        ss<< std::hex << mColor.getHex();
        std::string res ( ss.str() );
        while(res.size() < 6) res+="0";
        mInput.setInitialText(ofToUpper(res));
        mInput.setBackgroundColor(mColor);
        mInput.setTextInactiveColor(mColor.getBrightness() < BRIGHTNESS_THRESHOLD ? ofColor::white : ofColor::black);
    }
    
    private:

        ofColor mColor;
        ofColor gColor;
    
        struct {
            shared_ptr<ofImage> image;
            ofRectangle rect;
        } rainbow;
    
        bool mShowPicker;
        ofColor pickerBorder;
        ofRectangle pickerRect;
        ofRectangle gradientRect;
    
        ofVbo vbo;
        vector<ofVec2f> gPoints;
        vector<ofFloatColor> gColors;
    
    bool rainbowClicked;
    bool gradientRectClicked;
    
        static const int BRIGHTNESS_THRESHOLD = 185;

};

