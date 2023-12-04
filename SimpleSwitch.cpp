
// QUICK SOLUTION ALERT:
// BEFORE RUNNING ON ARDUINO IDE, FIX IT BACK TO <>
 #include <SimpleSwitch.h>
//#include "SimpleSwitch.h"

SimpleSwitch::SimpleSwitch(void)
{
    // Initialize object with default settings...
    Define(DEFAULT_PIN_NUMBER,DEFAULT_ACTION_TYPE,DEFAULT_CIRCUIT_TYPE,DEFAULT_DEBOUNCE_TIME,DEFAULT_NUMBER_OF_POSITIONS);
}

SimpleSwitch::SimpleSwitch(int pin = DEFAULT_PIN_NUMBER, int actionType = DEFAULT_ACTION_TYPE, int circuitType = DEFAULT_CIRCUIT_TYPE, int debounceTime = DEFAULT_DEBOUNCE_TIME, int numberOfPositions = DEFAULT_NUMBER_OF_POSITIONS)
{
    // Initialize object with user provided settings...
    Define(pin,actionType,circuitType,debounceTime,numberOfPositions);
}

int SimpleSwitch::pin(void)
{
    return _pin;
}

void SimpleSwitch::Define(int pin, int actionType = DEFAULT_ACTION_TYPE, int circuitType = DEFAULT_CIRCUIT_TYPE, int debounceTime  = DEFAULT_DEBOUNCE_TIME, int numberOfPositions  = DEFAULT_NUMBER_OF_POSITIONS)
{
	_pin = pin;
	_actionType = actionType;
	_circuitType = circuitType;
	_numOfPositions = numberOfPositions;
	
	// Action defaults to Momentary
	if(_actionType < MOMENTARY_ACTION || _actionType > CIRCULAR_ACTION)
		_actionType = DEFAULT_ACTION_TYPE;
	
	// Circuit type defaults to closed
	if(_circuitType < DEFAULT_OPEN_PULL_DOWN || _circuitType > DEFAULT_CLOSED_PULL_UP)
		_circuitType = DEFAULT_CIRCUIT_TYPE;
		
	if(_circuitType == DEFAULT_OPEN_PULL_DOWN)
    {
        circuitOn = HIGH;
		circuitOff = LOW;
        pinMode(_pin, INPUT);
	}
	
	if(_circuitType == DEFAULT_CLOSED_PULL_DOWN)
	{
        circuitOn = LOW;
		circuitOff = HIGH;
        pinMode(_pin,INPUT);
	}
    
    if(_circuitType == DEFAULT_OPEN_PULL_UP)
    {
        circuitOn = LOW;
        circuitOff = HIGH;
        pinMode(_pin, INPUT_PULLUP);
    }
    
    if(_circuitType == DEFAULT_CLOSED_PULL_UP)
    {
        circuitOn = HIGH;
        circuitOff = LOW;
        pinMode(_pin, INPUT_PULLUP);
    }

	currState = circuitOff;        	// current state of foot switch
	lastState = circuitOff;    		// last state of the foot switch
	currReading = circuitOff;      	// current reading from foot switch pin (used for debouncing)
	lastReading = circuitOff;    	// last reading from foot switch pin (used for debouncing)
	lastDebounceTime = 0; 			// last time input pin changed (milliseconds)
	debounceDelay = debounceTime;  	// maximum debounce time  (milliseconds)
	currPosition = 0;				// start at first position
    stateChanged = false;
    was_toggled = false;
	changedPos = false;		// used to flag position changes in circular switches
	buttonState = false;
}

void SimpleSwitch::debounce(void)
{
	// check switch state...
     currReading = digitalRead(_pin);
    
    was_toggled = false;
    stateChanged = false;
    changedPos = false; //(?) not sure about this one...
  
    if (currReading != lastReading)
    {
        lastDebounceTime = millis();
    }
	  
    if ((millis() - lastDebounceTime) >= debounceDelay)
    {
        currState = currReading;

        switch(_actionType)
        {
            case MOMENTARY_ACTION:
            {
                if(currState == circuitOn)
                {
                    buttonState = true;			// THE BUTTON IS CURRENTLY PRESSED
                }
                else
                {
                    buttonState = false;
                }

                if( currState != lastState )
                {
                    stateChanged = true;
                }
               else
               {
                   stateChanged = false;
               }
                break;
            }
            
            case TOGGLE_ACTION:
            {
                if( (currState == circuitOn) && (lastState == circuitOff) )
                {
                  buttonState = !buttonState;
                  was_toggled = true;
                }
                else
                {
                    was_toggled = false;
                }
                
                break;
            }
            
            case CIRCULAR_ACTION:		// THE BUTTON WAS PRESSED AND WILL SWITCH TO THE NEXT POSITION
            {
                if( (currState == circuitOn) && (lastState == circuitOff) )
                {
                   currPosition++;
                   if(currPosition >= _numOfPositions)
                        currPosition = 0;
                    buttonState = true;
                    changedPos = true;
                }
                
                if( currState == lastState )
                    changedPos = false;
                
                if( (currState == circuitOff) && (lastState == circuitOn) )
                {
                    buttonState = false;
                    changedPos = false;
                }
                break;
            }
        }
        lastState = currState;
    }
    lastReading = currReading;
}

bool SimpleSwitch::on(void)
{
	return buttonState;
}

int SimpleSwitch::position(void)
{
	return currPosition;
}

bool SimpleSwitch::changedState(void)
{
    return stateChanged;
}

bool SimpleSwitch::wasToggled(void)
{
    return was_toggled;
}

bool SimpleSwitch::changedPosition(void)
{
	return changedPos;
}

void SimpleSwitch::setPosition(int p)
{
    if(p >= 0 && p < _numOfPositions)
        currPosition = p;
}


// FIX FOR DOUBLE HIT ON TOGGLE_ACTION (MAYBE CIRCULAR_ACTION TOO)
    // Test and see if it should stay...
    // The reason is that these states should only
    // be valid immediatly after a successful debounce
    // The way it is set right now, they only get reset if
    // the switch debounces again, which can lead to double
    // hits if calling code relies on wasToggled() or changedPos()
