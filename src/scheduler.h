/***************************************************************************
 *
 * Author: "Sjors H.W. Scheres"
 * MRC Laboratory of Molecular Biology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * This complete copyright notice must be included in any revised version of the
 * source code. Additional authorship citations may be added, but existing
 * author citations must be preserved.
 ***************************************************************************/

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "src/time.h"
#include "src/pipeliner.h"

class SchedulerFloatVariable
{
	public:
	RFLOAT value, original_value;

	SchedulerFloatVariable() {};

	SchedulerFloatVariable(RFLOAT _value, RFLOAT _original_value)
	{
		value = _value;
		original_value = _original_value;
	}
};

class SchedulerBooleanVariable
{
	public:
	bool value, original_value;

	SchedulerBooleanVariable() {};

	SchedulerBooleanVariable(RFLOAT _value, RFLOAT _original_value)
	{
		value = _value;
		original_value = _original_value;
	}
};

class SchedulerStringVariable
{
	public:
	FileName value, original_value;

	SchedulerStringVariable() {};

	SchedulerStringVariable(FileName _value, FileName _original_value)
	{
		value = _value;
		original_value = _original_value;
	}
};

bool isBooleanVariable(std::string name);
bool isFloatVariable(std::string name);
bool isStringVariable(std::string name);
bool isOperator(std::string name);

#define SCHEDULE_BOOLEAN_OPERATOR_AND "bool_op_and"
#define SCHEDULE_BOOLEAN_OPERATOR_OR  "bool_op_or"
#define SCHEDULE_BOOLEAN_OPERATOR_NOT "bool_op_not"
#define SCHEDULE_BOOLEAN_OPERATOR_GT_VAR "bool_op_gt_var"
#define SCHEDULE_BOOLEAN_OPERATOR_LT_VAR "bool_op_lt_var"
#define SCHEDULE_BOOLEAN_OPERATOR_EQ_VAR "bool_op_eq_var"
#define SCHEDULE_BOOLEAN_OPERATOR_GT_CONST "bool_op_gt_const"
#define SCHEDULE_BOOLEAN_OPERATOR_LT_CONST "bool_op_lt_const"
#define SCHEDULE_BOOLEAN_OPERATOR_EQ_CONST "bool_op_eq_const"
#define SCHEDULE_BOOLEAN_OPERATOR_FILE_EXISTS "bool_op_file_exists"
#define SCHEDULE_FLOAT_OPERATOR_PLUS_VAR "float_op_plus_float"
#define SCHEDULE_FLOAT_OPERATOR_MINUS_VAR "float_op_minus_float"
#define SCHEDULE_FLOAT_OPERATOR_MULT_VAR "float_op_mult_float"
#define SCHEDULE_FLOAT_OPERATOR_DIVIDE_VAR "float_op_divide_float"
#define SCHEDULE_FLOAT_OPERATOR_PLUS_CONST "float_op_plus_const"
#define SCHEDULE_FLOAT_OPERATOR_MINUS_CONST "float_op_minus_const"
#define SCHEDULE_FLOAT_OPERATOR_MULT_CONST "float_op_mult_const"
#define SCHEDULE_FLOAT_OPERATOR_DIVIDE_CONST "float_op_div_by_const"
#define SCHEDULE_FLOAT_OPERATOR_DIVIDE_CONST_INV "float_op_div_const_by"
#define SCHEDULE_STRING_OPERATOR_TOUCH_FILE "string_op_touch_file"
#define SCHEDULE_STRING_OPERATOR_COPY_FILE "string_op_copy_file"
#define SCHEDULE_STRING_OPERATOR_MOVE_FILE "string_op_move_file"
#define SCHEDULE_STRING_OPERATOR_DELETE_FILE "string_op_delete_file"
#define SCHEDULE_WAIT_OPERATOR_SINCE_LAST_TIME "wait_since_last_time"
#define SCHEDULE_EXIT_OPERATOR "exit"

// A class that performs operators on variables
class SchedulerOperator
{
	public:
	std::string type, input1, input2, output;

	public:

	bool performOperation() const;

	SchedulerOperator() {};

	SchedulerOperator(std::string _type, std::string _input1="undefined", std::string _input2="undefined", std::string _output="undefined");

	// Generate a meaningful current_name for the operator
	std::string getName();

};

#define SCHEDULE_NODE_JOB_MODE_NEW "new"
#define SCHEDULE_NODE_JOB_MODE_CONTINUE "continue"
#define SCHEDULE_NODE_JOB_MODE_OVERWRITE "overwrite"

class SchedulerJob
{
	public:
	std::string current_name, mode;
	bool job_has_started;

	public:

	SchedulerJob() {};

	SchedulerJob(std::string _name, std::string _mode, bool _has_started = false)
	{
		current_name = _name;
		mode = _mode;
		job_has_started = _has_started;
	}

	// Perform operation and return TRUE if not a JOB; just return FALSE if a JOB
	bool performOperation();
};


// A class that defines the edges between a graph that defines execution order, where the nodes are individual JOB instances
// An edge can also be a fork, where the output is controlled through a boolean variable
class SchedulerEdge
{
	public:
	std::string inputNode, outputNode, outputNodeFalse;
	std::string myBooleanVariable;
	bool is_fork;

	std::string getOutputNode() const;

	SchedulerEdge(std::string _input, std::string _output, bool _is_fork, std::string _mybool, std::string _output_if_false)
	{
		inputNode = _input;
		outputNode= _output;
		is_fork = _is_fork;
		outputNodeFalse = _output_if_false;
		myBooleanVariable = _mybool;
	}

	SchedulerEdge(std::string _input, std::string _output)
	{
		inputNode = _input;
		outputNode= _output;
		is_fork = false;
		outputNodeFalse = "undefined";
		myBooleanVariable = "undefined";
	}

};


class Schedule
{

public:

	std::string current_node, original_start_node;
	std::string name, email_address;

	std::map<std::string, SchedulerJob> jobs;
	std::vector<SchedulerEdge> edges;

	PipeLine schedule_pipeline;

public:

	Schedule()
	{
		clear();
	}

	void clear();

	void setName(std::string _name)
	{
		name = _name;
		schedule_pipeline.setName(_name + "schedule");
	}

	void read(FileName fn = "");

	void write(FileName fn = "");

    void reset();

    void setCurrentNode(std::string _name);
    void setOriginalStartNode(std::string _name);

    bool gotoNextNode();
    bool gotoNextJob();

	bool isNode(std::string name);
	bool isJob(std::string name);

	std::string findJobByCurrentName(std::string name);

    // Get/set Variables and Operators(scheduler_floats is only visible in this file!)
    float getFloatVariableValue(std::string name);
    float getFloatOriginalVariableValue(std::string name);
    void setFloatVariableValue(std::string name, RFLOAT val);
    void setFloatOriginalVariableValue(std::string name, RFLOAT val);

    bool getBooleanVariableValue(std::string name);
    bool getBooleanOriginalVariableValue(std::string name);
    void setBooleanVariableValue(std::string name, bool val);
    void setBooleanOriginalVariableValue(std::string name, bool val);

    std::string getStringVariableValue(std::string name);
    std::string getStringOriginalVariableValue(std::string name);
    void setStringVariableValue(std::string name, std::string val);
    void setStringOriginalVariableValue(std::string name, std::string val);

    void setOperatorParameters(std::string name, std::string type, std::string input1, std::string input2, std::string output);
    void getOperatorParameters(std::string name, std::string &type, std::string &input1, std::string &input2, std::string &output);

    // Get vectors with current Variables / Operators
    std::map<std::string, SchedulerFloatVariable> getCurrentFloatVariables();
    std::map<std::string, SchedulerBooleanVariable> getCurrentBooleanVariables();
    std::map<std::string, SchedulerStringVariable> getCurrentStringVariables();
    std::map<std::string, SchedulerOperator> getCurrentOperators();

    // Get/set operators

    // Add variables
    void setVariable(std::string name, FileName value); // (Add new one if exists, otherwise set value)
	void addFloatVariable(std::string name, RFLOAT value);
    void addBooleanVariable(std::string name, bool value);
    void addStringVariable(std::string name, FileName value);

    // Add operators (of any kind), also adds its corresponding node
    void addOperator(std::string type, std::string input_name, std::string input2_name, std::string output_name);

    // Add a new job, also adds its corresponding node
    void addJob(RelionJob &myjob, std::string jobname, std::string mode);

    void addExitNode();

    // Remove variables/operators/jobs
    void removeVariable(std::string name);
    void removeOperator(std::string name);
    void removeJob(std::string name);


    void sendEmail(std::string message);

    // Add edges and forks in between the nodes
    void addEdge(std::string inputnode_name, std::string outputnode_name);
    void addFork(std::string inputnode_name, std::string mybool_name, std::string outputnode_name, std::string outputnode_name_if_false );

    // Test integrity of the Schedule. Warn for unused variables, nodes, etc.
    bool isValid();

    // This function fixes the dependency of newly generated jobs, as determined by the pipeline_schedule
    RelionJob copyNewJobFromSchedulePipeline(FileName original_job_name);

    // Modify a job to set variables from the Scheduler
    void setVariablesInJob(RelionJob &job, FileName original_job_name);

    // Run the Schedule
    void run(PipeLine &pipeline);

    // Abort a running schedule
    void abort();

};


#endif /* SCHEDULER_H_ */
