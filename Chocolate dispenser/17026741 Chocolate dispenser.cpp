#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum state { Out_Of_Chocolate, No_Credit, Has_Credit, Dispenses_Chocolate, Maintenance_Mode }; //these are all the different stages that the machine can be in at a moment in time.

class StateContext; //forward declaration to the state context class

class State //this class contains information on the states
{
protected:
	StateContext* CurrentContext; //a pointer of type state context will identify the machines current context
public:
	State(StateContext* Context) { CurrentContext = Context; } //casting of the method to adhere to the current context
	virtual ~State(void) {} //destructor
};

class StateContext //this class stores data on the different contexts available to the machine
{
protected:
	State* CurrentState = nullptr; //pointer of type state, identifies the machines current state
	int stateIndex = 0; //each state will be assigned a numerical value that can be used for diagnostic and identification purspose
	vector<State*> availableStates; //a vector of type state to store all the available states
public:
	virtual ~StateContext(void); //destructor
	virtual void setState(state newState); //a method with the purpose of setting the machine to a certain state when ever called
	virtual int getStateIndex(void); //method to allow access to the state index in the protected section of this class
};

StateContext::~StateContext(void)
{
	for (int i = 0; i < this->availableStates.size(); i++) delete this->availableStates[i];
	this->availableStates.clear(); //keep clearing available state
}

// method to set the machine to a new state
void StateContext::setState(state newState)
{
	this->CurrentState = availableStates[newState]; //look into the available states and set the new state to the one requested
	this->stateIndex = newState; //also change state index to that of the new state
}

int StateContext::getStateIndex(void) //allows access to state index
{
	return this->stateIndex;
}

class Transition //this class will contain all the different processes that can take place within the machine
{
public:
	virtual bool insertMoney(int) { cout << "Error!" << endl; return false; } //this method will allow for the addition of credits into the machine
	virtual bool makeSelection(int) { cout << "Error!" << endl; return false; } //this will allow the user to select how many bars of chocolate they would like to buy
	virtual bool moneyRejected(void) { cout << "Error!" << endl; return false; } //this method will be called will the funds inserted are rejected by the machine
	virtual bool addChocolate(int) { cout << "Error!" << endl; return false; } //this will allow for the addition of chocolate to the machines inventory 
	virtual bool dispense(void) { cout << "Error!" << endl; return false; } //this will be called when the machine dispenses chocolate
	virtual bool enterPin(int pin) { cout << "Error!" << endl; return false; } //this will allow the user to enter a pin. if authorised, they will be directed to maintenace mode
	virtual bool exit(void) { cout << "Error!" << endl; return false; } //this will allow the user to exit maintenace mode
};

class ChocoState : public State, public Transition //this inherits from the state and transition classes which allows for the association between states and ways in which to transition between them
{
public:
	ChocoState(StateContext* Context) : State(Context) {} //methods and interactions must be recast as the state changes
};

class OutOfChocolate : public ChocoState //this class contains all the operable functions for when the machine if out pf chocolate
{
public:
	OutOfChocolate(StateContext* Context) : ChocoState(Context) {} //recasting when state changes
	bool enterPin(int pin); 
	bool moneyRejected(void);
};

class NoCredit : public ChocoState //contains all the operable functions for when the user has no credit
{
public:
	NoCredit(StateContext* Context) : ChocoState(Context) {} //recasting to accomodate for changes in state
	bool insertMoney(int credit);
	bool enterPin(int pin);
};

class HasCredit : public ChocoState //contains all the operable functions for when the user has credit
{
public:
	HasCredit(StateContext* Context) : ChocoState(Context) {} //recasting to accomodate for changes in state
	bool insertMoney(int credit);
	bool makeSelection(int option);
	bool moneyRejected(void);
};

class DispensesChocolate : public ChocoState //contains function which will allow the machine to dispense chocolate when needed
{
public:
	DispensesChocolate(StateContext* Context) : ChocoState(Context) {}
	bool dispense(void);
};

class MaintenanceMode : public ChocoState //this class contains all available functions when in mainteneance mode
{
public:
	MaintenanceMode(StateContext* Context) : ChocoState(Context) {} //recasting to accomodate for changes in state
	bool exit(void);
	bool addChocolate(int number);
};

class Chocolate_Dispenser : public StateContext, public Transition //chocolate dispenser class. inherits form the stateContext and transition classes. will contain the final methode calls for the different transitions
{
	//declaring the following classes as 'freind classes' will allow the methods within them unrestricted access to the data in the provate section of the (current) chocolate dispenser class
	friend class OutOfChocolate;
	friend class NoCredit;
	friend class HasCredit;
	friend class DispensesChocolate;
	friend class MaintenanceMode;
private:
	int inventory = 0; //number of chocolate
	int credit = 0; //a measure of the number of bars that can be purchased and not money
	int pin = 54321; //secret pin for maintenance mode - *DO NOT CHANGE*
public:
	Chocolate_Dispenser(void); //cuntructor for the chocolate dispenser
	bool insertMoney(int credit); //this method will allow for the addition of credits into the machine
	bool makeSelection(int option); //this will allow the user to select how many bars of chocolate they would like to buy
	bool moneyRejected(void); //this method will be called will the funds inserted are rejected by the machine
	bool addChocolate(int number); //this will allow for the addition of chocolate to the machines inventory 
	bool dispense(void); //this will be called when the machine dispenses chocolate
	bool enterPin(int pin); //this will allow the user to enter a pin. if authorised, they will be directed to maintenace mode
	bool exit(void); //this will allow the user to exit maintenace mode
};

//create a chocolate dispenser
Chocolate_Dispenser::Chocolate_Dispenser(void)
{
	//store all the different state into the available states vector
	this->availableStates.push_back(new OutOfChocolate(this));
	this->availableStates.push_back(new NoCredit(this));
	this->availableStates.push_back(new HasCredit(this));
	this->availableStates.push_back(new DispensesChocolate(this));
	this->availableStates.push_back(new MaintenanceMode(this));

	this->setState(Out_Of_Chocolate); //set the machines state to out of chocolate
}

//the following are method calls for the different transition functions
bool Chocolate_Dispenser::insertMoney(int credit) 
{
	return ((ChocoState*)CurrentState)->insertMoney(credit); 
}
bool Chocolate_Dispenser::makeSelection(int option)
{
	return ((ChocoState*)CurrentState)->makeSelection(option);
}
bool Chocolate_Dispenser::moneyRejected(void)
{
	return ((ChocoState*)CurrentState)->moneyRejected();
}
bool Chocolate_Dispenser::addChocolate(int number)
{
	return ((ChocoState*)CurrentState)->addChocolate(number);
}
bool Chocolate_Dispenser::dispense(void)
{
	return ((ChocoState*)CurrentState)->dispense();
}
bool Chocolate_Dispenser::enterPin(int pin)
{
	return ((ChocoState*)CurrentState)->enterPin(pin);
}
bool Chocolate_Dispenser::exit(void)
{
	return ((ChocoState*)CurrentState)->exit();
}


bool OutOfChocolate::enterPin(int pin) //out of chocolate and the user inserts a pin
{
	if (((Chocolate_Dispenser*)CurrentContext)->pin != pin) //if the incorect pin has been entered, alert the user and dont allow them into maintenace mode
	{
		cout << "======================================" << endl;
		cout << "[Incorrect Pin!]" << endl;
		cout << "======================================" << endl;
		return true;
	}

	//otherwise allow the user into maintenace mode
	cout << "======================================" << endl;
	cout << "[Entering maintenance mode]" << endl;
	cout << "======================================" << endl;
	CurrentContext->setState(Maintenance_Mode); //set the machines new state to maintenance mode
	return true;
}
bool OutOfChocolate::moneyRejected(void) //when out of chocolate the user inserts money thats been rejected
{
	//let the user know that the funds they've inserted have been rejected by the machine
	cout << "======================================" << endl;
	cout << "[Funds rejected]" << endl;
	cout << "======================================" << endl;
	((Chocolate_Dispenser*)CurrentContext)->credit = 0; //set the the users available credits to 0
	CurrentContext->setState(No_Credit); //set the machines current state to no credit
	return true;
}
bool NoCredit::enterPin(int pin) //when the machine has no credit and the enter pin method is called
{
	if (((Chocolate_Dispenser*)CurrentContext)->pin != pin) //if the incorect pin has been entered, alert the user and dont allow them into maintenace mode
	{
		cout << "======================================" << endl;
		cout << "[incorrect pin!]" << endl;
		cout << "======================================" << endl;
		return true;
	}
	//otherwise allow the user into maintenace mode
	cout << "======================================" << endl;
	cout << "[Entering maintenance mode]" << endl;
	cout << "======================================" << endl;
	CurrentContext->setState(Maintenance_Mode); //set the machines new state to maintenance mode
	return true;
}
bool NoCredit::insertMoney(int credit) //when the machine has no credit and the user inserts money
{
	cout << "======================================" << endl; //give them confirmation that funds are in fact being added
	cout << "[Adding Credit...]" << endl;
	((Chocolate_Dispenser*)CurrentContext)->credit += credit; //increment their availabe credit by however much they've inserted
	cout << "Credit: " << ((Chocolate_Dispenser*)CurrentContext)->credit << endl; //show them how much credit they have
	cout << "======================================" << endl;
	CurrentContext->setState(Has_Credit); //set the machines current stae to 'Has credit'
	return true;
}
bool HasCredit::insertMoney(int credit) //when there already is credit and the user inserts money
{
	cout << "======================================" << endl; //inform them that you are adding the funds
	cout << "[Adding Credit...]" << endl;
	((Chocolate_Dispenser*)CurrentContext)->credit += credit; //add the inserted amount to the current amount
	cout << "Credit: " << ((Chocolate_Dispenser*)CurrentContext)->credit << endl; //display the new balance
	cout << "======================================" << endl;
	CurrentContext->setState(Has_Credit); //set the machines state to 'Has credit'
	return true;
}
bool DispensesChocolate::dispense(void) //when dipensing chocolate from the machine
{
	cout << "======================================" << endl;
	cout << "[dispensing Chocolate]" << endl; //alert them that you are dispensing the chocolate
	cout << "Inventory: " << ((Chocolate_Dispenser*)CurrentContext)->inventory << endl; //display the remaining inventory
	cout << "Credit: " << ((Chocolate_Dispenser*)CurrentContext)->credit << endl; //display the remaining balance (credits)
	cout << "======================================" << endl;
	
	if (((Chocolate_Dispenser*)CurrentContext)->inventory == 00) { CurrentContext->setState(Out_Of_Chocolate);} //if there if nothing in the machines inventory, set the machines state to 'out of chocolate'
	else if (((Chocolate_Dispenser*)CurrentContext)->credit == 0 && ((Chocolate_Dispenser*)CurrentContext)->inventory > 0) CurrentContext->setState(No_Credit); //if there are no more credits at the users disposal but there remains chococlate in the machines inventory, set the machines current state to 'no credit'
	else{CurrentContext->setState(Has_Credit);} //otherwise, set the machines state to 'has credit' 
	return true;
}
bool HasCredit::makeSelection(int option) //if the user has credit and is making a selection
{
	cout << "======================================" << endl;
	cout << "[you have selected " << option << " bars of chocolate]" << endl; //let them know how many bars of chocolate they have elected to dispence
	cout << "======================================" << endl;

	if (((Chocolate_Dispenser*)CurrentContext)->inventory < option) //if they've requested more chocolate than is availabe
	{ 
		cout << "======================================" << endl;
		cout << "[not enough in stock]" << endl; cout << "[inventory: " << ((Chocolate_Dispenser*)CurrentContext)->inventory << " ]" << endl; //alert them that there isn't enough in stock and show them the current inventory
		cout << "======================================" << endl;
		return true;
	}

	if (((Chocolate_Dispenser*)CurrentContext)->credit < option)  //if their request costs more than their current balance
	{ 
		cout << "======================================" << endl;
		cout << "[Error! insufficient funds]" << endl; //alert them that they do not have enough credits to make the purchase
		cout << "======================================" << endl;
		return true;
	}
	//if the previous case are not an issue then alert the user that their selection is being processed
	cout << "======================================" << endl;
	cout << "[processing selection]" << endl;
	cout << "======================================" << endl;
	((Chocolate_Dispenser*)CurrentContext)->inventory -= option; //remove the amount that they've selected from the inventory
	((Chocolate_Dispenser*)CurrentContext)->credit -= option; //charge them for their purchase by decrementing from their balance

	CurrentContext->setState(Dispenses_Chocolate); //set the machines current sate to 'dispense chocolate'
	return true;
}
bool HasCredit::moneyRejected() //when the user has credit and their money has been rejected
{
	//let the user know that the funds they've inserted have been rejected by the machine
	cout << "======================================" << endl;
	cout << "[Funds rejected]" << endl;
	cout << "======================================" << endl;
	((Chocolate_Dispenser*)CurrentContext)->credit = 0; //set their remaining balance to 0
	CurrentContext->setState(No_Credit); //set the machines current state to 'no credit'
	return true;
}
bool MaintenanceMode::addChocolate(int number) //when adding chocolate in the maintenance mode
{
	cout << "======================================" << endl;
	cout << "[adding " << number << " chocolate bars to inventory]" << endl; //alert the user that you are adding the request number of chocolate to the machine
	((Chocolate_Dispenser*)CurrentContext)->inventory += number; //increment the inventory by the inserted number
	cout << "inventory: " << ((Chocolate_Dispenser*)CurrentContext)->inventory << endl; //display how much in stock
	cout << "======================================" << endl;
	return true;
}
bool MaintenanceMode::exit(void) //when exiting maintenance mode
{
	cout << "======================================" << endl;
	cout << "[Exiting maintenance mode]" << endl; //alert the user that you are exiting maintenance mode
	cout << "Inventory: " << ((Chocolate_Dispenser*)CurrentContext)->inventory << endl; //display how much is in stock
	cout << "Credit: " << ((Chocolate_Dispenser*)CurrentContext)->credit << endl; //display current balance
	cout << "======================================" << endl;

	if (((Chocolate_Dispenser*)CurrentContext)->inventory > 0) { CurrentContext->setState(No_Credit); } //if there are one or more bars of chocolate in the machines inventory then set the machines current state to 'no credit'
	else if (((Chocolate_Dispenser*)CurrentContext)->inventory == 0) { CurrentContext->setState(Out_Of_Chocolate); } //if their is no stock in the machines inventory then set the machines state to out of chocolate
	return true;
}


int main(void)
{
	Chocolate_Dispenser newDispenser;
	
	/*newDispenser.enterPin(43523);
	newDispenser.enterPin(54321);
	newDispenser.addChocolate(30);
	newDispenser.insertMoney(27);
	newDispenser.exit();
	newDispenser.insertMoney(27);
	newDispenser.moneyRejected();
	newDispenser.insertMoney(7);
	newDispenser.makeSelection(17);
	newDispenser.dispense();
	newDispenser.makeSelection(13);
	newDispenser.dispense();*/
	newDispenser.enterPin(43523);
	newDispenser.enterPin(54321);
	newDispenser.addChocolate(15);
	newDispenser.exit();
	newDispenser.insertMoney(2);
	newDispenser.makeSelection(3);
	newDispenser.makeSelection(2);
	newDispenser.dispense();

	return 0;
}