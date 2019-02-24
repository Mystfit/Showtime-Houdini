// Showtime includes
#include <showtime/Showtime.h>

// Houdini includes
#include <UT/UT_DSOVersion.h>
#include <PRM/PRM_Include.h>
#include <OP/OP_Director.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_Director.h>

// DSO includes
#include "ShowtimeManager.h"


// showtimeIndirect is an array of indices that are used to access parameters
// quickly. The indices are initialized by the EVAL_FLOAT methods in the
// header file.
int *ShowtimeManager::showtimeIndirect = 0;

// Constructor for new object class
ShowtimeManager::ShowtimeManager(OP_Network *net, const char *name, OP_Operator *op) : OBJ_Geometry(net, name, op)
{
	if (!showtimeIndirect) showtimeIndirect = allocIndirect(1);
	
	//Start showtime lib
	zst_init(name, true);

	//Update our showtime nodes from the houdini network
	update_nodes();
}




// virtual destructor for new object class
ShowtimeManager::~ShowtimeManager()
{
	zst_destroy();
}

// here is the name of the parameter that is used by the shake object
static PRM_Name addressName("server_address", "Server address");
static PRM_Name connectName("connect", "Connect");

static PRM_Default addressDefault(0, "127.0.0.1");

// this is the template list that defines the new parameters that are
// used by the shake object.
static PRM_Template     templatelist[] =
{
	PRM_Template(PRM_STRING, 1, &addressName, &addressDefault),
	PRM_Template(PRM_CALLBACK, 1, &connectName, 0, 0, 0, ShowtimeManager::connect),
	PRM_Template()
};

// this function returns the OP_TemplatePair that combines the parameters
// of this object with those of its ancestors in the (object type hierarchy)
OP_TemplatePair * ShowtimeManager::buildTemplatePair(OP_TemplatePair *prevstuff)
{
	OP_TemplatePair     *manager, *geo;
	// Here, we have to "inherit" template pairs from geometry and beyond. To
	// do this, we first need to instantiate our template list, then add the
	// base class templates.
	manager = new OP_TemplatePair(templatelist, prevstuff);
	geo = new OP_TemplatePair(OBJ_Geometry::getTemplateList(OBJ_PARMS_PLAIN), manager);
	return geo;
}

int ShowtimeManager::connect(void * data, int index, float time, const PRM_Template * tplate)
{
	auto node = (ShowtimeManager*)(data);
	UT_String address;
	node->evalString(address, "server_address", 0, time);
	zst_join(address.c_str());
	return 1;
}

void ShowtimeManager::node_created_callback(OP_Node * caller, void * callee, OP_EventType type, void * data)
{
}

void ShowtimeManager::node_destroyed_callback(OP_Node * caller, void * callee, OP_EventType type, void * data)
{
}

void ShowtimeManager::update_nodes()
{
	auto director = OPgetDirector();
	director->traverseChildren([](OP_Node & node, void * data) -> bool {
		ShowtimeManager* manager = (ShowtimeManager*)data;
		manager->register_hou_node(node);
		return false;
	}, this, true);
}

void ShowtimeManager::register_hou_node(OP_Node & node)
{
	node.addOpInterest(this, [](OP_Node *caller, void *callee, OP_EventType type, void *data) {
		OP_Node* callee_node = (OP_Node*)callee;
		UT_String caller_path;
		UT_String callee_path;
		UT_String data_node_path;

		std::cout << "Caller: " << caller->getFullPath(caller_path) << std::endl;
		std::cout << "Callee: " << callee_node->getFullPath(callee_path) << std::endl;

		switch (type) {
		case OP_CHILD_CREATED:
			std::cout << "Node created: " << ((OP_Node*)data)->getFullPath(data_node_path) << std::endl;
			break;
		case OP_CHILD_DELETED:
			std::cout << "Node deleted: " << ((OP_Node*)data)->getFullPath(data_node_path) << std::endl;
			break;
		default:
			std::cout << "Event type: " << OPeventToString(type) << std::endl;
			break;
		}
	});
}

// the myConstructor method is used to create new objects of the correct
// type from the OperatorTable.
OP_Node *
ShowtimeManager::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
	return new ShowtimeManager(net, name, op);
}

// this function installs the new object in houdini's object table.
void newObjectOperator(OP_OperatorTable *table)
{
	table->addOperator(new OP_Operator("showtime_manager", "Showtime manager",
		ShowtimeManager::myConstructor,
		ShowtimeManager::buildTemplatePair(0),
		ShowtimeManager::theChildTableName,
		0, 
		1,
		0)
	);
}