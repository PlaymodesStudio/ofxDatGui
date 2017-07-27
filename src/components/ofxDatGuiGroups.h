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
#include "ofxDatGuiLabel.h"
#include "ofxDatGuiButton.h"
#include "ofxDatGuiSlider.h"
#include "ofxDatGuiTextInput.h"
#include "ofxDatGuiFRM.h"
#include "ofxDatGui2dPad.h"
#include "ofxDatGuiColorPicker.h"
#include "ofxDatGuiMatrix.h"
#include "ofxDatGuiTimeGraph.h"
#include "ofxDatGuiScrollView.h"

class ofxDatGuiGroup : public ofxDatGuiButton {

    public:
    
        ofxDatGuiGroup(string label) : ofxDatGuiButton(label)
        {
            mIsExpanded = false;
        }
    
        ~ofxDatGuiGroup()
        {
        // color pickers are deleted automatically when the group is destroyed //
            for (auto i:children) if (i->getType() != ofxDatGuiType::COLOR_PICKER) delete i;
        }
    
        void setPosition(int x, int y)
        {
            ofxDatGuiComponent::setPosition(x, y);
            layout();
        }
    
        void expand()
        {
            mIsExpanded = true;
            layout();
        }
    
        void toggle()
        {
            mIsExpanded = !mIsExpanded;
            layout();
        }
    
        void collapse()
        {
            mIsExpanded = false;
            layout();
        }
    
        int getHeight()
        {
            return mHeight;
        }
    
        bool getIsExpanded()
        {
            return mIsExpanded;
        }
    
        int size()
        {
            return children.size();
        }
        int clear()
        {
            children.clear();
        }

    
        void draw()
        {
            if (mVisible){
                ofPushStyle();
                ofxDatGuiButton::draw();
                if (mIsExpanded) {
                    int mHeight = mStyle.height;
                    ofSetColor(mStyle.guiBackground, mStyle.opacity);
                    ofDrawRectangle(x, y+mHeight, mStyle.width, mStyle.vMargin);
                    for(int i=0; i<children.size(); i++) {
                        mHeight += mStyle.vMargin;
                        children[i]->draw();
                        mHeight += children[i]->getHeight();
                        if (i == children.size()-1) break;
                        ofSetColor(mStyle.guiBackground, mStyle.opacity);
                        ofDrawRectangle(x, y+mHeight, mStyle.width, mStyle.vMargin);
                    }
                    ofSetColor(mIcon.color);
                    mIconOpen->draw(x+mIcon.x, y+mIcon.y, mIcon.size, mIcon.size);
                    for(int i=0; i<children.size(); i++) children[i]->drawColorPicker();
                }   else{
                    ofSetColor(mIcon.color);
                    mIconClosed->draw(x+mIcon.x, y+mIcon.y, mIcon.size, mIcon.size);
                }
                ofPopStyle();
            }
        }
    
    protected:
    
        void layout()
        {
            mHeight = mStyle.height + mStyle.vMargin;
            for (int i=0; i<children.size(); i++) {
                if (children[i]->getVisible() == false) continue;
                if (mIsExpanded == false){
                    children[i]->setPosition(x, y + mHeight);
                }   else{
                    children[i]->setPosition(x, y + mHeight);
                    mHeight += children[i]->getHeight() + mStyle.vMargin;
                }
                if (i == children.size()-1) mHeight -= mStyle.vMargin;
            }
        }
    
        void onMouseRelease(ofPoint m)
        {
            if (mFocused){
            // open & close the group when its header is clicked //
                ofxDatGuiComponent::onFocusLost();
                ofxDatGuiComponent::onMouseRelease(m);
                mIsExpanded ? collapse() : expand();
            // dispatch an event out to the gui panel to adjust its children //
                if (internalEventCallback != nullptr){
                    ofxDatGuiInternalEvent e(ofxDatGuiEventType::DROPDOWN_TOGGLED, mIndex);
                    internalEventCallback(e);
                }
            }
        }
    
        void dispatchInternalEvent(ofxDatGuiInternalEvent e)
        {
            if (e.type == ofxDatGuiEventType::VISIBILITY_CHANGED) layout();
            internalEventCallback(e);
        }
    
        int mHeight;
        shared_ptr<ofImage> mIconOpen;
        shared_ptr<ofImage> mIconClosed;
        bool mIsExpanded;
    
};

class ofxDatGuiFolder : public ofxDatGuiGroup {

    public:
    
        ofxDatGuiFolder(string label, ofColor color = ofColor::white) : ofxDatGuiGroup(label)
        {
        // all items within a folder share the same stripe color //
            mStyle.stripe.color = color;
            mType = ofxDatGuiType::FOLDER;
            setTheme(ofxDatGuiComponent::theme.get());
        }
    
        void setTheme(const ofxDatGuiTheme* theme)
        {
            setComponentStyle(theme);
            mIconOpen = theme->icon.groupOpen;
            mIconClosed = theme->icon.groupClosed;
            setWidth(theme->layout.width, theme->layout.labelWidth);
        // reassign folder color to all components //
            for(auto i:children) i->setStripeColor(mStyle.stripe.color);
        }
    
        void setWidth(int width, float labelWidth = 1)
        {
            ofxDatGuiComponent::setWidth(width, labelWidth);
            mLabel.width = mStyle.width;
            mLabel.rightAlignedXpos = mIcon.x - mLabel.margin;
            ofxDatGuiComponent::positionLabel();
        }
    
        void drawColorPicker()
        {
            for(int i=0; i<pickers.size(); i++) pickers[i]->drawColorPicker();
        }
    
        void dispatchButtonEvent(ofxDatGuiButtonEvent e)
        {
            if (buttonEventCallback != nullptr) {
                buttonEventCallback(e);
            }   else{
                ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
            }
        }
    
        void dispatchToggleEvent(ofxDatGuiToggleEvent e)
        {
            if (toggleEventCallback != nullptr) {
                toggleEventCallback(e);
        // allow toggle events to decay into button events //
            }   else if (buttonEventCallback != nullptr) {
                buttonEventCallback(ofxDatGuiButtonEvent(e.target));
            }   else{
                ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
            }
        }
    
        void dispatchSliderEvent(ofxDatGuiSliderEvent e)
        {
            if (sliderEventCallback != nullptr) {
                sliderEventCallback(e);
            }   else{
                ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
            }
        }
    
        void dispatchTextInputEvent(ofxDatGuiTextInputEvent e)
        {
            if (textInputEventCallback != nullptr) {
                textInputEventCallback(e);
            }   else{
                ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
            }
        }
    
        void dispatchColorPickerEvent(ofxDatGuiColorPickerEvent e)
        {
            if (colorPickerEventCallback != nullptr) {
                colorPickerEventCallback(e);
            }   else{
                ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
            }
        }
    
        void dispatch2dPadEvent(ofxDatGui2dPadEvent e)
        {
            if (pad2dEventCallback != nullptr) {
                pad2dEventCallback(e);
            }   else{
                ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
            }
        }
    
        void dispatchMatrixEvent(ofxDatGuiMatrixEvent e)
        {
            if (matrixEventCallback != nullptr) {
                matrixEventCallback(e);
            }   else{
                ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
            }
        }
    
        ofxDatGuiToggle* getToggleAt(int index)
        {
            return static_cast<ofxDatGuiToggle*>(children[index]);
        }


    /*
        component add methods
    */

        ofxDatGuiLabel* addLabel(string label)
        {
            ofxDatGuiLabel* lbl = new ofxDatGuiLabel(label);
            lbl->setStripeColor(mStyle.stripe.color);
            attachItem(lbl);
            return lbl;
        }

        ofxDatGuiButton* addButton(string label)
        {
            ofxDatGuiButton* button = new ofxDatGuiButton(label);
            button->setStripeColor(mStyle.stripe.color);
            button->onButtonEvent(this, &ofxDatGuiFolder::dispatchButtonEvent);
            attachItem(button);
            return button;
        }
    
        ofxDatGuiToggle* addToggle(string label, bool enabled = false)
        {
            ofxDatGuiToggle* toggle = new ofxDatGuiToggle(label, enabled);
            toggle->setStripeColor(mStyle.stripe.color);
            toggle->onToggleEvent(this, &ofxDatGuiFolder::dispatchToggleEvent);
            attachItem(toggle);
            return toggle;
        }
    
        ofxDatGuiSlider* addSlider(string label, float min, float max)
        {
            ofxDatGuiSlider* slider = addSlider(label, min, max, (max+min)/2);
            return slider;
        }

        ofxDatGuiSlider* addSlider(string label, float min, float max, double val)
        {
            ofxDatGuiSlider* slider = new ofxDatGuiSlider(label, min, max, val);
            slider->setStripeColor(mStyle.stripe.color);
            slider->onSliderEvent(this, &ofxDatGuiFolder::dispatchSliderEvent);
            attachItem(slider);
            return slider;
        }

        ofxDatGuiSlider* addSlider(ofParameter<int> & p){
            ofxDatGuiSlider* slider = new ofxDatGuiSlider(p);
            slider->setStripeColor(mStyle.stripe.color);
            slider->onSliderEvent(this, &ofxDatGuiFolder::dispatchSliderEvent);
            attachItem(slider);
            return slider;
        }

        ofxDatGuiSlider* addSlider(ofParameter<float> & p){
            ofxDatGuiSlider* slider = new ofxDatGuiSlider(p);
            slider->setStripeColor(mStyle.stripe.color);
            slider->onSliderEvent(this, &ofxDatGuiFolder::dispatchSliderEvent);
            attachItem(slider);
            return slider;
        }
    
        ofxDatGuiTextInput* addTextInput(string label, string value)
        {
            ofxDatGuiTextInput* input = new ofxDatGuiTextInput(label, value);
            input->setStripeColor(mStyle.stripe.color);
            input->onTextInputEvent(this, &ofxDatGuiFolder::dispatchTextInputEvent);
            attachItem(input);
            return input;
        }
    
        ofxDatGuiColorPicker* addColorPicker(string label, ofColor color = ofColor::black)
        {
            shared_ptr<ofxDatGuiColorPicker> picker(new ofxDatGuiColorPicker(label, color));
            picker->setStripeColor(mStyle.stripe.color);
            picker->onColorPickerEvent(this, &ofxDatGuiFolder::dispatchColorPickerEvent);
            attachItem(picker.get());
            pickers.push_back(picker);
            return picker.get();
        }
    
        ofxDatGuiFRM* addFRM(float refresh = 1.0f)
        {
            ofxDatGuiFRM* monitor = new ofxDatGuiFRM(refresh);
            monitor->setStripeColor(mStyle.stripe.color);
            attachItem(monitor);
            return monitor;
        }

        ofxDatGuiBreak* addBreak()
        {
            ofxDatGuiBreak* brk = new ofxDatGuiBreak();
            attachItem(brk);
            return brk;
        }
    
        ofxDatGui2dPad* add2dPad(string label)
        {
            ofxDatGui2dPad* pad = new ofxDatGui2dPad(label);
            pad->setStripeColor(mStyle.stripe.color);
            pad->on2dPadEvent(this, &ofxDatGuiFolder::dispatch2dPadEvent);
            attachItem(pad);
            return pad;
        }

        ofxDatGuiMatrix* addMatrix(string label, int numButtons, bool showLabels = false)
        {
            ofxDatGuiMatrix* matrix = new ofxDatGuiMatrix(label, numButtons, showLabels);
            matrix->setStripeColor(mStyle.stripe.color);
            matrix->onMatrixEvent(this, &ofxDatGuiFolder::dispatchMatrixEvent);
            attachItem(matrix);
            return matrix;
        }
    
        ofxDatGuiWaveMonitor* addWaveMonitor(string label, float frequency, float amplitude)
        {
            ofxDatGuiWaveMonitor* monitor = new ofxDatGuiWaveMonitor(label, frequency, amplitude);
            monitor->setStripeColor(mStyle.stripe.color);
            attachItem(monitor);
            return monitor;
        }
    
        ofxDatGuiValuePlotter* addValuePlotter(string label, float min, float max)
        {
            ofxDatGuiValuePlotter* plotter = new ofxDatGuiValuePlotter(label, min, max);
            plotter->setStripeColor(mStyle.stripe.color);
            attachItem(plotter);
            return plotter;
        }
    
        void attachItem(ofxDatGuiComponent* item)
        {
            item->setIndex(children.size());
            item->onInternalEvent(this, &ofxDatGuiFolder::dispatchInternalEvent);
            children.push_back(item);
        }
    
        ofxDatGuiComponent* getComponent(ofxDatGuiType type, string label)
        {
            for (int i=0; i<children.size(); i++) {
                if (children[i]->getType() == type){
                    if (children[i]->is(label)) return children[i];
                }
            }
            return NULL;
        }

        static ofxDatGuiFolder* getInstance() { return new ofxDatGuiFolder("X"); }

    protected:
    
        vector<shared_ptr<ofxDatGuiColorPicker>> pickers;
    
};

class ofxDatGuiDropdownOption : public ofxDatGuiButton {

    public:
    
        ofxDatGuiDropdownOption(string label) : ofxDatGuiButton(label)
        {
            mType = ofxDatGuiType::DROPDOWN_OPTION;
            setTheme(ofxDatGuiComponent::theme.get());
        }
    
        void setTheme(const ofxDatGuiTheme* theme)
        {
            ofxDatGuiButton::setTheme(theme);
            mStyle.stripe.color = theme->stripe.dropdown;
        }
    
        void setWidth(int width, float labelWidth = 1)
        {
            ofxDatGuiComponent::setWidth(width, labelWidth);
            mLabel.width = mStyle.width;
            mLabel.rightAlignedXpos = mIcon.x - mLabel.margin;
            ofxDatGuiComponent::positionLabel();
        }

};

class ofxDatGuiDropdown : public ofxDatGuiGroup {

    public:

        ofxDatGuiDropdown(string label, const vector<string>& options = vector<string>()) : ofxDatGuiGroup(label)
        {
            mOption = 0;
            mType = ofxDatGuiType::DROPDOWN;
            for(int i=0; i<options.size(); i++){
                ofxDatGuiDropdownOption* opt = new ofxDatGuiDropdownOption(options[i]);
                opt->setIndex(children.size());
                opt->onButtonEvent(this, &ofxDatGuiDropdown::onOptionSelected);
                children.push_back(opt);
            }
            setTheme(ofxDatGuiComponent::theme.get());
            setLabelColor(getTheme()->color.label*2);

            
        }
    
        void addOption(string option)
        {
            ofxDatGuiDropdownOption* opt = new ofxDatGuiDropdownOption(option);
            opt->setIndex(children.size());
            opt->onButtonEvent(this, &ofxDatGuiDropdown::onOptionSelected);
            children.push_back(opt);
        }
    
        void addOption(string option, int position)
        {
            ofxDatGuiDropdownOption* opt = new ofxDatGuiDropdownOption(option);
            opt->setIndex(children.size());
            opt->onButtonEvent(this, &ofxDatGuiDropdown::onOptionSelected);
            children.insert(children.begin() + position, opt);
        }

    
        void setTheme(const ofxDatGuiTheme* theme)
        {
            setComponentStyle(theme);
            mIconOpen = theme->icon.groupOpen;
            mIconClosed = theme->icon.groupClosed;
            mStyle.stripe.color = theme->stripe.dropdown;
            setWidth(theme->layout.width, theme->layout.labelWidth);
        }
    
        void setWidth(int width, float labelWidth = 1)
        {
            ofxDatGuiComponent::setWidth(width, labelWidth);
            mLabel.width = mStyle.width;
            mLabel.rightAlignedXpos = mIcon.x - mLabel.margin;
            ofxDatGuiComponent::positionLabel();
        }
    
        void select(int cIndex)
        {
        // ensure value is in range //
            if (cIndex < 0 || cIndex >= children.size()){
                ofLogError() << "ofxDatGuiDropdown->select("<<cIndex<<") is out of range";
            }   else{
                setLabel(children[cIndex]->getLabel());
            }
        }

    
        ofxDatGuiDropdownOption* getChildAt(int index)
        {
            return static_cast<ofxDatGuiDropdownOption*>(children[index]);
        }
    
        ofxDatGuiDropdownOption* getSelected()
        {
            return static_cast<ofxDatGuiDropdownOption*>(children[mOption]);
        }
    
    
        int getSelectedIndex()
        {
            int res = -1;
            string myLabel = getLabel();
            
            for(int i=0;i<children.size();i++)
            {
                if(children[i]->getLabel()==myLabel)
                {
                    res=i;
                }
            }
            return res;
        }
    
    int getNumOptions()
    {
        return children.size();
    }
    

        static ofxDatGuiDropdown* getInstance() { return new ofxDatGuiDropdown("X"); }
    
    private:
    
        void onOptionSelected(ofxDatGuiButtonEvent e)
        {
            for(int i=0; i<children.size(); i++) if (e.target == children[i]) mOption = i;
            setLabel(children[mOption]->getLabel());
            setLabelColor(getTheme()->color.label*2);
            collapse();
            if (dropdownEventCallback != nullptr) {
                ofxDatGuiDropdownEvent e1(this, mIndex, mOption);
                dropdownEventCallback(e1);
            }   else{
                ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
            }
        }
    
        int mOption;
    
};


class ofxDatGuiParagraphInput : public ofxDatGuiTextInputField{
public:
    
    ofxDatGuiParagraphInput() : ofxDatGuiTextInputField()
    {
//        paragraph.setAlignment(ofxParagraph::ALIGN_LEFT);
        mPadding = 2;
//        paragraph.drawBorder(ofColor::fromHex(0x777777));
        mFocused = false;
//        height = paragraph.getHeight();
    }
    
    void setTheme(const ofxDatGuiTheme* theme)
    {
        font = theme->font.ptr;
//        paragraph.setFont(theme->font.ptr);
//        paragraph.setLeading(theme->font.size *.65);
//        paragraph.setSpacing(theme->font.size *.7);
//        paragraph.setIndent(0);
//        paragraph.setBorderPadding(mPadding);
//        setHeight(paragraph.getHeight());
//        paragraph.setColor(theme->color.textInput.text);
//        mInputRect.height = theme->layout.height - (theme->layout.padding * 2);
        color.active.background = theme->color.textInput.backgroundOnActive;
        color.inactive.background = theme->color.inputAreaBackground;
        color.active.text = theme->color.label;
        color.inactive.text = theme->color.textInput.text;
        color.highlight = theme->color.textInput.highlight;
//        mUpperCaseText = theme->layout.textInput.forceUpperCase;
//        mHighlightPadding = theme->layout.textInput.highlightPadding;
//        setText(mText);
    }
    
    void setInitialText(string text)
    {
        mText = text;
        mTextChanged = true;
        setHeight(((getNumLines()) * font->getLineHeight()) + mPadding*2);
    }
    
    void setCursorToEnd()
    {
        lineNum = getNumLines() - 1;
        setCursorIndex(mText.size());
    }
    
    void setText(string text)
    {
        mText = text;
        mTextChanged = true;
        setHeight(((getNumLines()+1) * font->getLineHeight()) + mPadding*2);
        
        ofxDatGuiInternalEvent e(ofxDatGuiEventType::INPUT_CHANGED, 0);
        internalEventCallback(e);
    }
    
    void setWidth(int w)
    {
//        paragraph.setWidth(w - mPadding*2);
        ofxDatGuiTextInputField::setWidth(w);
    }
    
    void setHeight(int h)
    {
        if(h != height){
//            mInputRect.height = h + paragraph.getStringHeight()*.5;
            height = h;
            ofNotifyEvent(heightChanged, h, this);
        }
    }
    
    int getNumLines(){
        return ofStringTimesInString(mText, "\n") + 1;
    }
    
    int getHeight()
    {
        return height;
    }
    
    void setPosition(int x, int y)
    {
//        paragraph.setPosition(x + mPadding, y + paragraph.getStringHeight()*1.5);
//        paragraph.setLeading(20);
//        paragraph.setSpacing(2);
        ofxDatGuiTextInputField::setPosition(x, y);
    }

    void draw()
    {
        ofPushStyle();
        // draw the input field background //
        ofSetColor(color.inactive.background);
        ofDrawRectangle(mInputRect);

        //draw paragraph
        ofSetColor(color.active.text);
        if((ofGetFrameNum() % 40) < 20){
            int lineStart = mText.size() - inversedFind(mText, "\n", mText.size() - mCursorIndex);
            if(lineStart > mCursorIndex) lineStart = 0;
            int widthFromLineStart = font->width(mText.substr(lineStart, mCursorFromLineStart));
            ofPoint linePos;
            linePos.x = mInputRect.x + mPadding + widthFromLineStart;
            linePos.y = mInputRect.y + (0.5*font->getLineHeight()) + (lineNum * font->getLineHeight());
            ofPushStyle();
            ofSetColor(ofColor::white);
            ofSetLineWidth(1);
            ofDrawLine(linePos.x, linePos.y - font->getLineHeight()/2, linePos.x, linePos.y + font->getLineHeight()/2);
            ofPopStyle();
        }
        font->draw(mText, mInputRect.x + mPadding, mInputRect.y + font->getLineHeight());
        ofPopStyle();
    }
    
    void onKeyPressed(int key)
    {
        switch(key){
            case OF_KEY_SHIFT:
            case OF_KEY_LEFT_SHIFT:
            case OF_KEY_RIGHT_SHIFT:
            case OF_KEY_ALT:
            case OF_KEY_CONTROL:
            case OF_KEY_COMMAND:
                return;
        }
        if (key == OF_KEY_BACKSPACE){
            // delete character at cursor position //
            if (mCursorIndex > 0) {
                if(mText[mCursorIndex-1] == '\n') lineNum -= 1;
                setText(mText.substr(0, mCursorIndex - 1) + mText.substr(mCursorIndex));
                setCursorIndex(mCursorIndex - 1);
            }
        } else if (key == OF_KEY_RETURN) {
            setText(mText.substr(0, mCursorIndex) + '\n' + mText.substr(mCursorIndex));
            lineNum += 1;
            setCursorIndex(mCursorIndex + 1);
        } else if (key == OF_KEY_LEFT) {
            int newCursor = max( (int) mCursorIndex - 1, 0);
            int lineStart = mText.size() - inversedFind(mText, "\n", mText.size() - mCursorIndex);
            if(lineStart == mCursorIndex){
                lineNum -= 1;
            }
            setCursorIndex(newCursor);
        } else if (key == OF_KEY_RIGHT) {
            int newCursor = min( mCursorIndex + 1, (unsigned int) mText.size());
            int lineStart = mText.size() - inversedFind(mText, "\n", mText.size() - newCursor);
            if(lineStart == newCursor){
                lineNum += 1;
            }
            setCursorIndex(newCursor);
        } else if (key == OF_KEY_UP) {
            if(lineNum > 0){
                int lineStart = inversedFind(mText, "\n", mText.size() - mCursorIndex);
                int previousLineStart = mText.size() - inversedFind(mText, "\n", lineStart + 1);
                if(previousLineStart > mCursorIndex) previousLineStart = 0;
                int newPos = min(previousLineStart + mCursorFromLineStart, (int)mText.size() - lineStart - 1);// (mCursorIndex - (mText.size() - lineStart));
                setCursorIndex(max((unsigned int) newPos, (unsigned int) 0));
                lineNum -= 1;
            }
        } else if (key == OF_KEY_DOWN) {
            if(lineNum < getNumLines()-1){
                int nextlineStart = mText.find("\n", mCursorIndex) + 1;
                int nextLineEnd = mText.find("\n", nextlineStart);
                int newPos = min((unsigned int)nextlineStart + mCursorFromLineStart, (unsigned int)nextLineEnd);
                setCursorIndex(min((unsigned int) newPos, (unsigned int) mText.size()));
                lineNum += 1;
            }
        } else if (key == 'v' && ofGetKeyPressed(OF_KEY_COMMAND))  {
            mText += ofGetWindowPtr()->getClipboardString();
            setCursorIndex(mCursorIndex + ofGetWindowPtr()->getClipboardString().size());
        } else{
            // insert character at cursor position //
            mText = mText.substr(0, mCursorIndex) + (char)key + mText.substr(mCursorIndex);
            setCursorIndex(mCursorIndex + 1);
        }
        mHighlightText = false;
        setText(mText);
    }
    
    void setCursorIndex(int index){
        int lineStart = mText.size() - inversedFind(mText, "\n", mText.size() - index);
        if(lineStart > index) lineStart = 0;
        mCursorFromLineStart = index - lineStart;
        ofxDatGuiTextInputField::setCursorIndex(index);
    }
    
    int inversedFind(string s, string toSearch, int initialPos){
        reverse(s.begin(), s.end());
        return s.find(toSearch, initialPos);
    }
    
    ofEvent<int>    heightChanged;
    
private:
    shared_ptr<ofxSmartFont>    font;
    int height;
    int mPadding;
    int lineNum;
    int mCursorFromLineStart;
};


class ofxDatGuiParagraph : public ofxDatGuiGroup{
public:
    
    ofxDatGuiParagraph(string label, string initalText = "This is a paragraph") : ofxDatGuiGroup(label)
    {
        
        mType = ofxDatGuiType::PARAGRAPH;
//        ofxDatGuiParagraphInput* opt = new ofxDatGuiParagraphInput();
//        children.push_back(opt);
        mInput.onInternalEvent(this, &ofxDatGuiParagraph::onInputChanged);
        setTheme(ofxDatGuiComponent::theme.get());
        setLabelColor(getTheme()->color.label*2);
        mInput.setInitialText(initalText);
        mInput.setCursorToEnd();
        ofAddListener(mInput.heightChanged, this, &ofxDatGuiParagraph::heightChangedListener);
    }
    
    void setTextWithoutEvent(string text){
        mInput.setInitialText(text);
    }
   
    void setTheme(const ofxDatGuiTheme* theme)
    {
        setComponentStyle(theme);
        mIconOpen = theme->icon.groupOpen;
        mIconClosed = theme->icon.groupClosed;
        mStyle.stripe.color = theme->stripe.dropdown;
        mInput.setTheme(theme);
        setWidth(theme->layout.width, theme->layout.labelWidth);
    }

    void setWidth(int width, float labelWidth = 1)
    {
        ofxDatGuiComponent::setWidth(width, labelWidth);
        mLabel.width = mStyle.width;
        mLabel.rightAlignedXpos = mIcon.x - mLabel.margin;
        ofxDatGuiComponent::positionLabel();
        mInput.setWidth(width);
    }
    
    void setPosition(int x, int y)
    {
        ofxDatGuiComponent::setPosition(x, y);
        
        ofxDatGuiGroup::setPosition(x, y);
        mInput.setPosition(x, y + mStyle.height);
        
        layout();
    }
    
    void expand()
    {
        mIsExpanded = true;
        layout();
        mInput.onFocus();
    }
    
    void toggle()
    {
        mIsExpanded = !mIsExpanded;
        layout();
    }
    
    void collapse()
    {
        mIsExpanded = false;
        layout();
        mInput.onFocusLost();
    }
    
    void draw()
    {
        if (mVisible){
            ofPushStyle();
            ofxDatGuiButton::draw();
            if (mIsExpanded) {
                mInput.draw();
                int mHeight = mStyle.height;
                ofSetColor(mStyle.guiBackground, mStyle.opacity);
                ofDrawRectangle(x, y+mHeight, mStyle.width, mStyle.vMargin);
                for(int i=0; i<children.size(); i++) {
                    mHeight += mStyle.vMargin;
                    children[i]->draw();
                    mHeight += children[i]->getHeight();
                    if (i == children.size()-1) break;
                    ofSetColor(mStyle.guiBackground, mStyle.opacity);
                    ofDrawRectangle(x, y+mHeight, mStyle.width, mStyle.vMargin);
                }
                ofSetColor(mIcon.color);
                mIconOpen->draw(x+mIcon.x, y+mIcon.y, mIcon.size, mIcon.size);
            }   else{
                ofSetColor(mIcon.color);
                mIconClosed->draw(x+mIcon.x, y+mIcon.y, mIcon.size, mIcon.size);
            }
            ofPopStyle();
        }
    }
    
    void onFocus()
    {
        mInput.onFocus();
        ofxDatGuiComponent::onFocus();
    }
    
    void onFocusLost()
    {
        ofxDatGuiComponent::onFocusLost();
    }
    
    static ofxDatGuiParagraph* getInstance() { return new ofxDatGuiParagraph("X");}

private:
    
    void heightChangedListener(int &height)
    {
        layout();
        if (internalEventCallback != nullptr){
            ofxDatGuiInternalEvent e(ofxDatGuiEventType::DROPDOWN_TOGGLED, mIndex);
            internalEventCallback(e);
        }
    }
    
    void onInputChanged(ofxDatGuiInternalEvent e){
        // dispatch event out to main application //
        if (paragraphEventCallback != nullptr) {
            ofxDatGuiParagraphEvent ev(this, mInput.getText());
            paragraphEventCallback(ev);
        }   else{
            ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
        }
    }
    
    void layout()
    {
        mHeight = mStyle.height + mStyle.vMargin;
        if(mIsExpanded){
            mHeight += mInput.getHeight();
        }
        
    }
    
    void onKeyPressed(int key)
    {
        if (mIsExpanded)
            mInput.onKeyPressed(key);
    }
    
    void onMouseEnter(ofPoint m)
    {
        ofxDatGuiGroup::onMouseEnter(m);
    }
    
    void onMouseLeave(ofPoint m)
    {
        ofxDatGuiGroup::onMouseLeave(m);
    }
    
    void onMousePress(ofPoint m)
    {
        ofxDatGuiGroup::onMousePress(m);
    }
    
    void onMouseRelease(ofPoint m)
    {
        if (mFocused){
            // open & close the group when its header is clicked //
            ofxDatGuiComponent::onMouseRelease(m);
            mIsExpanded ? collapse() : expand();
            if (internalEventCallback != nullptr){
                ofxDatGuiInternalEvent e(ofxDatGuiEventType::DROPDOWN_TOGGLED, mIndex);
                internalEventCallback(e);
            }
        }
    }
    
    void onMouseOutsidePress()
    {
        ofxDatGuiGroup::onMouseOutsidePress();
    }
    
    ofxDatGuiParagraphInput mInput;
};


