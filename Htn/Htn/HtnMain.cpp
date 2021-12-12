#include <iostream>
#include <vector>
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>

namespace htn
{
	template<typename T> using Array = std::vector<T>;

	using AttributeValue = float; //int32_t;
	using AttributeId = uint8_t;

	class WorldState
	{
	public:
		// Create WorldState based on attributeSet blueprint
		WorldState() = default;
		WorldState(std::vector<AttributeValue>& _attributes)
		{
			this->_attributes.reserve(_attributes.size());
			for (auto it : _attributes)
			{
				this->_attributes.push_back(it);
			}
		}

		AttributeValue GetRegisterAttributeValue() const
		{
			return _attributes[0];
		}
		AttributeValue GetAttributeValue(AttributeId attributeId) const
		{
			return _attributes[attributeId];
		}
		void SetAttributeValue(AttributeId attributeId, AttributeValue value)
		{
			_attributes[attributeId] = value;
		}

		AttributeId AddAttribute(AttributeValue initialValue)
		{
			_attributes.push_back(initialValue);

			return (AttributeId)(_attributes.size() - 1);
		}

	private:
		std::vector<AttributeValue>	_attributes;
	};

	class AttributeSet
	{
	public:

		bool ContainsAttribute(const std::string& attributeName) const
		{
			return  _attributeNameTranslation.find(attributeName) != _attributeNameTranslation.end();
		}

		AttributeId GetAttributeId(const std::string& attributeName) const
		{
			return _attributeNameTranslation.at(attributeName);
		}

		AttributeValue GetAttributeValue(const std::string& attributeName) const
		{
			return GetAttributeValue(GetAttributeId(attributeName));
		}
		AttributeValue GetAttributeValue(AttributeId attributeId) const
		{
			return _worldState.GetAttributeValue(attributeId);
		}


		void SetAttributeValue(const std::string& attributeName, AttributeValue value)
		{
			SetAttributeValue(GetAttributeId(attributeName), value);
		}
		void SetAttributeValue(AttributeId attributeId, AttributeValue value)
		{
			_worldState.SetAttributeValue(attributeId, value);
		}

		AttributeId AddAttribute(const std::string& attributeName, AttributeValue initialValue)
		{
			AttributeId id = _worldState.AddAttribute(initialValue);
			_attributeNameTranslation[attributeName] = id;

			return id;
		}

		WorldState& GetWorldState()
		{
			return _worldState;
		}

	private:
		WorldState _worldState;

		std::unordered_map<std::string, AttributeId> _attributeNameTranslation;
	};

	enum EOperationType : uint8_t
	{
		EAddParam = 0,
		ESubstractParam,
		EMultiplayParam,
		EDivideParam,

		EAddConst,
		ESubstractConst,
		EMultiplayConst,
		EDivideConst,

		EAddParamRegister,
		ESubstractParamRegister,
		EMultiplayParamRegister,
		EDivideParamRegister,

		EAddConstRegister,
		ESubstractConstRegister,
		EMultiplayConstRegister,
		EDivideConstRegister,

		ECopyParam, // copy value from one id to the another or write constant value
		ECopyConst, // copy value from one id to the another or write constant value

		// Conditional operations -> they modify one value
		ELessParam,
		ELessEqualParam,
		EEqualParam,
		EMoreParam,
		EMoreEqualParam,

		ELessConst,
		ELessEqualConst,
		EEqualConst,
		EMoreConst,
		EMoreEqualConst,

		ELessParamRegister,
		ELessEqualParamRegister,
		EEqualParamRegister,
		EMoreParamRegister,
		EMoreEqualParamRegister,

		ELessConstRegister,
		ELessEqualConstRegister,
		EEqualConstRegister,
		EMoreConstRegister,
		EMoreEqualConstRegister,

		ESqrtParam,
		ESqrtConst,

		ECosParam,
		ECosConst,

		ESinParam,
		ESinConst,

		EMAX,
	};

	class Operation
	{
	public:
		Operation(EOperationType operationType, AttributeId lhs, AttributeValue rhs)
			: operationType(operationType)
			, leftParamId(lhs)
			, constantValue(rhs)
		{

		}

		union
		{
			AttributeValue constantValue;			// 32
			AttributeId rightParamId;				// 8
		};

		// what operation to perform
		AttributeId leftParamId;				// 8
		EOperationType operationType;			// 8


		void Perform(WorldState& values) const
		{
			switch (operationType)
			{
			case EOperationType::EAddParam:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) + values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::ESubstractParam:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) - values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::EMultiplayParam:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) * values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::EDivideParam:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) / values.GetAttributeValue(rightParamId));
				break;

			case EOperationType::EAddConst:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) + constantValue);
				break;
			case EOperationType::ESubstractConst:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) - constantValue);
				break;
			case EOperationType::EMultiplayConst:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) * constantValue);
				break;
			case EOperationType::EDivideConst:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) / constantValue);
				break;

			case EOperationType::EAddParamRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) + values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::ESubstractParamRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) - values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::EMultiplayParamRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) * values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::EDivideParamRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) / values.GetAttributeValue(rightParamId));
				break;

			case EOperationType::EAddConstRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) + constantValue);
				break;
			case EOperationType::ESubstractConstRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) - constantValue);
				break;
			case EOperationType::EMultiplayConstRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) * constantValue);
				break;
			case EOperationType::EDivideConstRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) / constantValue);
				break;

			case EOperationType::ECopyParam:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(rightParamId));
				break;// copy value from one id to the another or write constant value
			case EOperationType::ECopyConst:
				values.SetAttributeValue(leftParamId, constantValue);
				break;// copy value from one id to the another or write constant value

			case EOperationType::ELessParam:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) < values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::ELessEqualParam:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) <= values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::EEqualParam:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) == values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::EMoreParam:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) > values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::EMoreEqualParam:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) >= values.GetAttributeValue(rightParamId));
				break;

			case EOperationType::ELessConst:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) < constantValue);
				break;
			case EOperationType::ELessEqualConst:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) <= constantValue);
				break;
			case EOperationType::EEqualConst:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) == constantValue);
				break;
			case EOperationType::EMoreConst:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) > constantValue);
				break;
			case EOperationType::EMoreEqualConst:
				values.SetAttributeValue(leftParamId, values.GetAttributeValue(leftParamId) >= constantValue);
				break;

			case EOperationType::ELessParamRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) < values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::ELessEqualParamRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) <= values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::EEqualParamRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) == values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::EMoreParamRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) > values.GetAttributeValue(rightParamId));
				break;
			case EOperationType::EMoreEqualParamRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) >= values.GetAttributeValue(rightParamId));
				break;

			case EOperationType::ELessConstRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) < constantValue);
				break;
			case EOperationType::ELessEqualConstRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) <= constantValue);
				break;
			case EOperationType::EEqualConstRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) == constantValue);
				break;
			case EOperationType::EMoreConstRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) > constantValue);
				break;
			case EOperationType::EMoreEqualConstRegister:
				values.SetAttributeValue(0, values.GetAttributeValue(leftParamId) >= constantValue);
				break;

			case EOperationType::ESqrtParam:
				values.SetAttributeValue(leftParamId, std::sqrt(values.GetAttributeValue(rightParamId)));
				break;
			case EOperationType::ESqrtConst:
				values.SetAttributeValue(leftParamId, std::sqrt(constantValue));
				break;

			case EOperationType::ECosParam:
				values.SetAttributeValue(leftParamId, std::cos(values.GetAttributeValue(rightParamId)));
				break;
			case EOperationType::ECosConst:
				values.SetAttributeValue(leftParamId, std::cos(constantValue));
				break;

			case EOperationType::ESinParam:
				values.SetAttributeValue(leftParamId, std::sin(values.GetAttributeValue(rightParamId)));
				break;
			case EOperationType::ESinConst:
				values.SetAttributeValue(leftParamId, std::sin(constantValue));
				break;

			default:
				std::cerr << "Wrong operation type used";
				break;
			}
		}
	};

	class OperationScript
	{
	public:

		void AddOperation(EOperationType operationType, AttributeId lhs, AttributeValue rhs)
		{
			operations.push_back(Operation(operationType, lhs, rhs));
		}

		AttributeValue Perform(WorldState& values) const
		{
			for (int i = 0; i < operations.size(); ++i)
			{
				operations[i].Perform(values);
			}
			return values.GetRegisterAttributeValue();
		}

	private:
		std::vector<Operation> operations;
	};
}

namespace htn
{
	using TaskId = int32_t;

	class PrimitiveTask
	{
	public:
		OperationScript precondition;
		OperationScript effect;
	};

	class CompoundMethod
	{
	public:
		CompoundMethod() = default;
		CompoundMethod(std::initializer_list<TaskId> initialTasks)
			: tasks(initialTasks)
		{
		}

		OperationScript utility;
		OperationScript effect;
		std::vector<TaskId> tasks;
	};

	class CompoundTask
	{
	public:
		std::vector<CompoundMethod> methods;
	};

	class Domain
	{
	public:

		bool IsPrimitiveTask(const TaskId taskID) const
		{
			return taskID < _primitiveTasks.size();
		}
		bool IsCompoundTask(const TaskId taskID) const
		{
			return !IsPrimitiveTask(taskID);
		}

		bool ContainsTask(const TaskId taskId) const
		{
			return taskId < _primitiveTasks.size()
				&& taskId - _primitiveTasks.size() < _compoundTasks.size();
		}
		bool ContainsTask(const std::string& name) const
		{
			return ContainsName(name) && ContainsTask(_taskNameToIdMap.at(name));
		}
		bool ContainsName(const std::string& name) const
		{
			return _taskNameToIdMap.find(name) != _taskNameToIdMap.end();
		}

		TaskId GetTaskId(const std::string& name) const
		{
			return _taskNameToIdMap.at(name);
		}
		const std::string& GetTaskName(const TaskId name) const
		{
			return _taskIdToNameMap.at(name);
		}

		PrimitiveTask& GetPrimitiveTask(const TaskId taskID)
		{
			return _primitiveTasks[taskID];
		}
		const PrimitiveTask& GetPrimitiveTask(const TaskId taskID) const
		{
			return _primitiveTasks[taskID];
		}

		CompoundTask& GetCompoundTask(const TaskId taskID)
		{
			return _compoundTasks[taskID - _primitiveTasks.size()];
		}
		const CompoundTask& GetCompoundTask(const TaskId taskID) const
		{
			return _compoundTasks[taskID - _primitiveTasks.size()];
		}

		CompoundTask& CreateCompoundTask(const std::string& name)
		{
			TaskId  taskId = _compoundTasks.size() + _primitiveTasks.size();
			_taskNameToIdMap[name] = taskId;
			_taskIdToNameMap[taskId] = name;

			_compoundTasks.push_back(CompoundTask());
			return _compoundTasks.back();
		}
		PrimitiveTask& CreatePrimitiveTask(const std::string& name)
		{
			_taskNameToIdMap[name] = _primitiveTasks.size();
			_taskIdToNameMap[_primitiveTasks.size()] = name;

			_primitiveTasks.push_back(PrimitiveTask());
			return _primitiveTasks.back();
		}

	private:
		std::vector<PrimitiveTask>	_primitiveTasks;
		std::vector<CompoundTask>	_compoundTasks;

		std::map<std::string, TaskId> _taskNameToIdMap;
		std::map<TaskId, std::string> _taskIdToNameMap;
	};

	class RestorePoint
	{
	public:

		WorldState worldState;
		std::vector<TaskId> plan;
		int32_t nextMethod;
		TaskId activeTask;

		explicit RestorePoint(const WorldState& InWorldState = WorldState())
			: worldState(InWorldState), nextMethod(0), activeTask(-1)
		{
		}

		RestorePoint(const RestorePoint& PreviousPoint, const htn::TaskId currentTask, const int32_t nextMethod)
			: worldState(PreviousPoint.worldState), plan(PreviousPoint.plan), nextMethod(nextMethod), activeTask(currentTask)
		{}
	};

	template<class MethodFinder>
	void ComputePlan(htn::TaskId rootTaskId, htn::AttributeSet& attributeSet, const htn::Domain& domain, std::vector<size_t>& outPlan)
	{
		std::vector<htn::TaskId> tasksToProcess;
		std::vector<htn::RestorePoint> restorePoints;
		htn::RestorePoint currentPoint(attributeSet.GetWorldState());

		////////////////////////////////
		tasksToProcess.clear();
		tasksToProcess.push_back(rootTaskId);

		int32_t nextMethodId = 0;

		MethodFinder methodFinder;

		while (tasksToProcess.size() > 0)
		{
			htn::TaskId currentTaskId = tasksToProcess.back();
			tasksToProcess.pop_back();

			bool shouldRestore = false;

			if (domain.IsCompoundTask(currentTaskId))
			{
				const htn::CompoundTask& compoundTask = domain.GetCompoundTask(currentTaskId);
				int32_t methodId = methodFinder(compoundTask, currentPoint.worldState, nextMethodId);

				if (methodId != -1)
				{
					const htn::CompoundMethod& method = compoundTask.methods[methodId];

					method.effect.Perform(currentPoint.worldState);

					restorePoints.push_back(htn::RestorePoint(currentPoint, currentTaskId, nextMethodId));

					nextMethodId = 0;

					for (int32_t taskIndex = method.tasks.size() - 1; taskIndex >= 0; --taskIndex)
					{
						tasksToProcess.push_back(method.tasks[taskIndex]);
					}
				}
				else
				{
					shouldRestore = true;
				}

			}
			else
			{
				const htn::PrimitiveTask& primitiveTask = domain.GetPrimitiveTask(currentTaskId);

				if (primitiveTask.precondition.Perform(currentPoint.worldState))
				{
					primitiveTask.effect.Perform(currentPoint.worldState);
					currentPoint.plan.push_back(currentTaskId);
				}
				else
				{
					shouldRestore = true;
				}
			}

			if (shouldRestore && restorePoints.size() > 0)
			{
				currentPoint = restorePoints.back();
				restorePoints.pop_back();

				nextMethodId = currentPoint.nextMethod + 1;
				tasksToProcess.push_back(currentPoint.activeTask);
			}
		}

		outPlan.clear();
		outPlan.insert(outPlan.begin(), currentPoint.plan.begin(), currentPoint.plan.end());
	}

	class MethodFinder_ByCondition
	{
	public:
		int32_t operator() (const CompoundTask& task, WorldState& worldState, int32_t nextMethod)
		{
			for (; nextMethod < task.methods.size(); ++nextMethod)
			{
				const CompoundMethod& method = task.methods[nextMethod];

				if (method.utility.Perform(worldState))
				{
					return nextMethod;
				}
			}
			return -1;
		}
	};

	class MethodFinder_ByBestUtility
	{
	public:
		int32_t operator() (const CompoundTask& task, WorldState& worldState, int32_t nextMethod)
		{
			int32_t bestMethodId = -1;
			AttributeValue bestMethodUtility = std::numeric_limits<AttributeValue>::min();

			for (int32_t i = nextMethod; i < task.methods.size(); ++i)
			{
				const CompoundMethod& method = task.methods[i];
				AttributeValue currentUtility = method.utility.Perform(worldState);

				if (currentUtility > bestMethodUtility)
				{
					bestMethodUtility = currentUtility;
					bestMethodId = i;
				}
			}

			return bestMethodId;
		}
	};
}

htn::AttributeSet PrepareAttributeSet()
{
	htn::AttributeSet attributeSet;

	attributeSet.AddAttribute("Register", 0);
	attributeSet.AddAttribute("A", 0);
	attributeSet.AddAttribute("B", 7);
	attributeSet.AddAttribute("C", 8);

	return attributeSet;
}

htn::Domain PrepareDomain()
{
	htn::Domain domain;
	domain.CreatePrimitiveTask("task1")
		.precondition.AddOperation(htn::EOperationType::ECopyConst, 0, 1);
	domain.CreatePrimitiveTask("task2")
		.precondition.AddOperation(htn::EOperationType::ECopyConst, 0, 1);
	domain.CreatePrimitiveTask("task3")
		.precondition.AddOperation(htn::EOperationType::ECopyConst, 0, 1);

	auto& compoundTask = domain.CreateCompoundTask("compoundTask1");
	{
		htn::CompoundMethod method({ 0,3 });
		method.utility.AddOperation(htn::EOperationType::ELessConstRegister, 1, 5);
		method.utility.AddOperation(htn::EOperationType::EMultiplayConstRegister, 0, 5);

		method.effect.AddOperation(htn::EOperationType::ESubstractConst, 1, -1);
		compoundTask.methods.push_back(method);
	}

	{
		htn::CompoundMethod method({ 2 });
		method.utility.AddOperation(htn::EOperationType::ECopyParam, 0, 1);
		compoundTask.methods.push_back(method);
	}

	return domain;
}
int main()
{
	htn::AttributeSet attributeSet = PrepareAttributeSet();
	htn::Domain domain = PrepareDomain();
	htn::TaskId rootTaskId = domain.GetTaskId("compoundTask1");
	std::vector<size_t> resultingPlan;

	htn::ComputePlan<htn::MethodFinder_ByBestUtility>(rootTaskId, attributeSet, domain, resultingPlan);

	for (int32_t i = 0; i < resultingPlan.size(); ++i)
	{
		std::cout << "task [" << i << "] = " << domain.GetTaskName(resultingPlan[i]) << std::endl;
	}
}