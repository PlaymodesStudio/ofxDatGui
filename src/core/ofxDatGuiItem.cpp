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

#include "ofxDatGuiItem.h"

ofxDatGuiItem::ofxDatGuiItem(string label, ofxDatGuiFont* font)
{
    mAlpha = 255;
    mVisible = true;
    mMouseOver = false;
    mMouseDown = false;
    mLabelMarginRight = 0;
    mLabelAlignment = ofxDatGuiAlignment::LEFT;
    mRetinaEnabled = (ofGetScreenWidth()>=2560 && ofGetScreenHeight()>=1600);
    mIcon.y = 8;
    mIcon.size = 10;
    mRow.height = 26;
    mRow.padding = 2;
    mRow.spacing = 1;
    mStripeWidth = 2;
    if (mRetinaEnabled){
        mIcon.y *=2;
        mIcon.size *=2;
        mRow.height *=2;
        mRow.padding*=2;
        mRow.spacing*=2;
        mStripeWidth*=2;
    }
    if (font!=nullptr) {
        mFont = font;
    }   else{
        mFont = new ofxDatGuiFont(mRetinaEnabled);
    }
    setLabel(label);
}

ofxDatGuiItem::~ofxDatGuiItem(){ }

/*
    ofxDatGuiFont
*/

ofRectangle ofxDatGuiFont::getStringBoundingBox(string str, int x, int y)
{
    if (tFont.isLoaded()){
        return tFont.getStringBoundingBox(str, x, y);
    }   else{
        return bFont.getBoundingBox(str, x, y);
    }
}

void ofxDatGuiFont::drawText(string text, ofColor color, int xpos, int ypos, bool highlight)
{
    ofPushStyle();
        if (highlight){
            ofRectangle hRect = getStringBoundingBox(text, xpos, ypos+labelHeight/2);
            hRect.x -= highlightPadding;
            hRect.width += highlightPadding*2;
            hRect.y -= highlightPadding;
            hRect.height += highlightPadding*2;
            ofSetColor(ofxDatGuiColor::TEXT_HIGHLIGHT);
            ofDrawRectangle(hRect);
        }
        ofSetColor(color);
        if (tFont.isLoaded()){
            tFont.drawString(text, xpos, ypos+labelHeight/2);
        }   else{
            if (mRetinaEnabled) ypos-=2;
            ofDrawBitmapString(text, xpos, ypos+labelHeight/2);
        }
    ofPopStyle();
}

void ofxDatGuiFont::drawLabel(string text, int xpos, int ypos)
{
    ofPushStyle();
        ofSetColor(ofxDatGuiColor::LABEL);
        if (tFont.isLoaded()){
            tFont.drawString(text, xpos, ypos+labelHeight/2);
        }   else{
            if (!mRetinaEnabled) ypos-=2;
            ofDrawBitmapString(text, xpos, ypos+labelHeight/2);
        }
    ofPopStyle();
}

/*
    instance methods
*/

void ofxDatGuiItem::setIndex(int index) { mId = index; }

void ofxDatGuiItem::setAlpha(int alpha)
{
    mAlpha = alpha;
    for (int i=0; i<children.size(); i++) children[i]->setAlpha(alpha);
}

void ofxDatGuiItem::setWidth(int w)
{
    mRow.width = w;
    mRow.lWidth=mRow.width*.35;
    mRow.inputX=mRow.lWidth;
    mRow.rWidth=mRow.width-mRow.inputX;
    mFont->labelX=(mRow.width*.03)+10;
    mIcon.x=mRow.width-(mRow.width*.05)-20;
    mSlider.width=mRow.rWidth*.7;
    mSlider.inputX=mRow.inputX+mSlider.width+mRow.padding;
    mSlider.inputWidth=mRow.rWidth-mSlider.width-mRow.padding;
    for (int i=0; i<children.size(); i++) children[i]->setWidth(w);
}

int ofxDatGuiItem::getHeight()
{
    return mRow.height;
}

void ofxDatGuiItem::setLabel(string label)
{
    mLabel = label;
    mLabelRect = mFont->getStringBoundingBox(mLabel, 0, 0);
}

string ofxDatGuiItem::getLabel()
{
    return mLabel;
}

void ofxDatGuiItem::setAlignment(ofxDatGuiAlignment align)
{
    mLabelAlignment = align;
    for (int i=0; i<children.size(); i++) children[i]->setAlignment(align);
}

/*
    virtual methods overridden in derived classes
*/

bool ofxDatGuiItem::getIsExpanded(){}
void ofxDatGuiItem::setVisible(bool visible) { mVisible = visible; }
bool ofxDatGuiItem::getVisible() { return mVisible; }
void ofxDatGuiItem::setStripeColor(ofColor color) { mStripeColor = color; }

void ofxDatGuiItem::setOriginX(int x)
{
    this->x = x;
    mLabelAreaWidth = mRow.lWidth;
    for(int i=0; i<children.size(); i++) children[i]->setOriginX(x);
}

void ofxDatGuiItem::setOriginY(int y)
{
    this->y = mOriginY = y;
    for(int i=0; i<children.size(); i++) children[i]->setOriginY(this->y + (mRow.height+mRow.spacing)*(i+1));
}

int ofxDatGuiItem::getOriginY()
{
    return mOriginY;
}

void ofxDatGuiItem::setPositionY(int ypos)
{
    this->y = ypos;
}

int ofxDatGuiItem::getPositionY()
{
    return y;
}

/*
    draw methods
*/

void ofxDatGuiItem::update()
{
// TODO call this when the component is created outside of a gui //
}

void ofxDatGuiItem::drawBkgd(ofColor color, int alpha)
{
    ofPushStyle();
        ofSetColor(color, alpha!=-1 ? alpha  : mAlpha);
        ofDrawRectangle(x, y, mRow.width, mRow.height);
    ofPopStyle();
}

void ofxDatGuiItem::drawLabel()
{
    drawLabel(mLabel);
}

void ofxDatGuiItem::drawLabel(string label)
{
    int lx;
    if (mLabelAlignment == ofxDatGuiAlignment::LEFT){
        lx = mFont->labelX;
    }   else if (mLabelAlignment == ofxDatGuiAlignment::CENTER){
        lx = mLabelAreaWidth/2-mLabelRect.width/2;
    }   else if (mLabelAlignment == ofxDatGuiAlignment::RIGHT){
        lx = mLabelAreaWidth-mLabelRect.width-mFont->labelX-mLabelMarginRight;
    }
    mFont->drawLabel(label, x+lx, y+(mRow.height/2));
}

void ofxDatGuiItem::drawStripe()
{
    ofPushStyle();
        ofSetColor(mStripeColor);
        ofDrawRectangle(x, y, mStripeWidth, mRow.height);
    ofPopStyle();
}

void ofxDatGuiItem::drawColorPicker() { }

/*
    events
*/

void ofxDatGuiItem::onMouseEnter(ofPoint m)
{
     mMouseOver = true;
}

void ofxDatGuiItem::onMouseLeave(ofPoint m)
{
     mMouseOver = false;
     mMouseDown = false;
}

void ofxDatGuiItem::onMousePress(ofPoint m)
{
    mMouseDown = true;
}

void ofxDatGuiItem::onMouseRelease(ofPoint m)
{
    mMouseDown = false;
}

void ofxDatGuiItem::onFocus() {}
void ofxDatGuiItem::onFocusLost() { }
void ofxDatGuiItem::onKeyPressed(int key) { }
void ofxDatGuiItem::onMouseDrag(ofPoint m) { }


