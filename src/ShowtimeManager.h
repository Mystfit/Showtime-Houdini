#pragma once
#include <OBJ/OBJ_Geometry.h>

class ShowtimeManager : public OBJ_Geometry
{
public:
	ShowtimeManager(OP_Network *net, const char *name, OP_Operator *op);
	virtual ~ShowtimeManager();
	static OP_Node *myConstructor(OP_Network *net, const char *name, OP_Operator *entry);
	static OP_TemplatePair *buildTemplatePair(OP_TemplatePair *prevstuff);
	static int connect(void *data, int index, float time, const PRM_Template *tplate);
	static void node_created_callback(OP_Node *caller, void *callee, OP_EventType type, void *data);
	static void node_destroyed_callback(OP_Node *caller, void *callee, OP_EventType type, void *data);
	void update_nodes();
	void register_hou_node(OP_Node & node);

private:
	static int *showtimeIndirect;
};
