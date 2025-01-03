#include <iostream>
#include <queue>
#include <map>
#include <fstream>
#include <array>
#include <vector>
#include <string>
using namespace std;

class RegisterStat
{
public:
    map<string, string> status;
    RegisterStat()
    {
        status["R0"] = "";
        status["R1"] = "";
        status["R2"] = "";
        status["R3"] = "";
        status["R4"] = "";
        status["R5"] = "";
        status["R6"] = "";
        status["R7"] = "";
    }
};
struct RegisterFile
{
    int val[8];
    int &operator[](int index)
    {
        val[0] = 0;
        return val[index];
    }
    RegisterFile()
    {
        for (int i = 0; i < 8; i++)
        {
            val[i] = 0;
        }
    }
};
vector<string> string_split(string s, string ss)
{
    vector<string> s_list;
    size_t pos = 0;
    string token;
    while ((pos = s.find(ss)) != string::npos)
    {
        token = s.substr(0, pos);
        s_list.push_back(token);
        s.erase(0, pos + ss.length());
    }
    s_list.push_back(s);

    return s_list;
}

class Instruction
{
public:
    string Inst;
    string op;
    int IMM;
    int label;
    int offset;
    string RD;
    string RS1;
    string RS2;
    Instruction(string Inst)
    {
        this->Inst = Inst;
        Instruction_Processing();
    }
    Instruction()
    {
        Inst = "";
        op = "";
        IMM = 0;
        label = 0;
        offset = 0;
        RD = "";
        RS1 = "";
        RS2 = "";
    }
    void Instruction_Processing()
    {
        Inst.erase(remove(Inst.begin(), Inst.end(), ','), Inst.end());
        vector<string> s_list = string_split(Inst, " ");
        op = s_list[0];
        if (op == "ADD" || op == "NAND" || op == "DIV") // NAND RD, Rs1, Rs2
        {
            RD = s_list[1];
            RS1 = s_list[2];
            RS2 = s_list[3];
        }
        else if (op == "ADDI") // ADDI RD, Rs1, imm
        {
            RD = s_list[1];
            RS1 = s_list[2];
            IMM = stoi(s_list[3]);
        }
        else if (op == "LOAD") // LOAD RD, offset(RS1)
        {
            vector<string> src_part = string_split(s_list[2], "(");
            RD = s_list[1];
            offset = stoi(src_part[0]);
            RS1 = src_part[1].substr(0, src_part[1].length() - 1);
        }
        else if (op == "STORE") // STORE RS2, offset(RS2)
        {
            vector<string> src_part = string_split(s_list[2], "(");
            RS2 = s_list[1];
            offset = stoi(src_part[0]);
            RS1 = src_part[1].substr(0, src_part[1].length() - 1);
        }
        else if (op == "BNE") // BNE RS1, RS2, offset
        {
            RS1 = s_list[1];
            RS2 = s_list[2];
            label = stoi(s_list[3]);
        }
        else if (op == "CALL") // CALL label
        {
            label = stoi(s_list[1]);
        }
        else if (op == "RET") // RET
        {
            return;
        }
    }
};

class UnitEntry
{
public:
    string unitID;
    string operation;
    int operand1;
    int operand2;
    string dependency1;
    string dependency2;
    int address;
    bool isActive;
    int startCycle;
    int instructionStart;
    int executionEndCycle;
    int resultValue;
    bool isCompleted;

    UnitEntry()
    {
        unitID = "";
        operation = "";
        operand1 = 0;
        operand2 = 0;
        dependency1 = "";
        dependency2 = "";
        address = 0;
        isActive = false;
        startCycle = 0;
        instructionStart = 0;
        executionEndCycle = 0;
        resultValue = 0;
        isCompleted = false;
    }
};

class FunctionalUnitManager
{

public:
    map<string, UnitEntry> unitRegistry;

    int loadCycles, storeCycles, branchCycles, callReturnCycles, addCycles, nandCycles, divideCycles;

    int loadUnits, storeUnits, branchUnits, callReturnUnits, addUnits, nandUnits, divideUnits;

    int activeLoadUnits = 0;
    int activeStoreUnits = 0;
    int activeBranchUnits = 0;
    int activeCallReturnUnits = 0;
    int activeAddUnits = 0;
    int activeNandUnits = 0;
    int activeDivideUnits = 0;

    FunctionalUnitManager()
    {
        cout << "Please specify the count for each functional unit\n";

        cout << "Number of LOAD units: ";
        cin >> loadUnits;
        while (loadUnits <= 0)
        {
            cout << "LOAD unit count must exceed 0.";
            cin >> loadUnits;
        }

        cout << "Number of STORE units: ";
        cin >> storeUnits;
        while (storeUnits <= 0)
        {
            cout << "STORE unit count must exceed 0.";
            cin >> storeUnits;
        }

        cout << "Number of BRANCH units: ";
        cin >> branchUnits;
        while (branchUnits <= 0)
        {
            cout << "BRANCH unit count must exceed 0.";
            cin >> branchUnits;
        }

        cout << "Number of CALL/RETURN units: ";
        cin >> callReturnUnits;
        while (callReturnUnits <= 0)
        {
            cout << "CALL/RETURN unit count must exceed 0.";
            cin >> callReturnUnits;
        }

        cout << "Number of ADD units: ";
        cin >> addUnits;
        while (addUnits <= 0)
        {
            cout << "ADD unit count must exceed 0.";
            cin >> addUnits;
        }

        cout << "Number of NAND units: ";
        cin >> nandUnits;
        while (nandUnits <= 0)
        {
            cout << "NAND unit count must exceed 0.";
            cin >> nandUnits;
        }

        cout << "Number of DIVIDE units: ";
        cin >> divideUnits;
        while (divideUnits <= 0)
        {
            cout << "DIVIDE unit count must exceed 0.";
            cin >> divideUnits;
        }

        // loadUnits = 1, storeUnits = 1, branchUnits = 1, callReturnUnits = 1, callReturnUnits = 1, addUnits = 1, nandUnits = 1, divideUnits = 1;

        // Initialize unit registry
        for (int i = 0; i < loadUnits; i++)
        {
            unitRegistry["LoadUnit" + to_string(i)] = UnitEntry();
            unitRegistry["LoadUnit" + to_string(i)].unitID = "LoadUnit" + to_string(i);
        }
        for (int i = 0; i < storeUnits; i++)
        {
            unitRegistry["StoreUnit" + to_string(i)] = UnitEntry();
            unitRegistry["StoreUnit" + to_string(i)].unitID = "StoreUnit" + to_string(i);
        }
        for (int i = 0; i < branchUnits; i++)
        {
            unitRegistry["BranchUnit" + to_string(i)] = UnitEntry();
            unitRegistry["BranchUnit" + to_string(i)].unitID = "BranchUnit" + to_string(i);
        }
        for (int i = 0; i < callReturnUnits; i++)
        {
            unitRegistry["CallReturnUnit" + to_string(i)] = UnitEntry();
            unitRegistry["CallReturnUnit" + to_string(i)].unitID = "CallReturnUnit" + to_string(i);
        }
        for (int i = 0; i < addUnits; i++)
        {
            unitRegistry["AddUnit" + to_string(i)] = UnitEntry();
            unitRegistry["AddUnit" + to_string(i)].unitID = "AddUnit" + to_string(i);
        }
        for (int i = 0; i < nandUnits; i++)
        {
            unitRegistry["NandUnit" + to_string(i)] = UnitEntry();
            unitRegistry["NandUnit" + to_string(i)].unitID = "NandUnit" + to_string(i);
        }
        for (int i = 0; i < divideUnits; i++)
        {
            unitRegistry["DivideUnit" + to_string(i)] = UnitEntry();
            unitRegistry["DivideUnit" + to_string(i)].unitID = "DivideUnit" + to_string(i);
        }

        // loadCycles = 1, storeCycles = 1, branchCycles = 1, callReturnCycles = 1, addCycles = 1, nandCycles = 1, divideCycles = 1;
        //  Request cycle counts
        cout << "\nNow, specify the cycle counts for each functional unit type.\n";

        cout << "LOAD unit cycles: ";
        cin >> loadCycles;
        while (loadCycles <= 0)
        {
            cout << "LOAD unit cycles must exceed 0. ";
            cin >> loadCycles;
        }

        cout << "STORE unit cycles: ";
        cin >> storeCycles;
        while (storeCycles <= 0)
        {
            cout << "STORE unit cycles must exceed 0.";
            cin >> storeCycles;
        }

        cout << "BRANCH unit cycles: ";
        cin >> branchCycles;
        while (branchCycles <= 0)
        {
            cout << "BRANCH unit cycles must exceed 0.";
            cin >> branchCycles;
        }

        cout << "CALL/RETURN unit cycles: ";
        cin >> callReturnCycles;
        while (callReturnCycles <= 0)
        {
            cout << "CALL/RETURN unit cycles must exceed 0.";
            cin >> callReturnCycles;
        }

        cout << "ADD unit cycles: ";
        cin >> addCycles;
        while (addCycles <= 0)
        {
            cout << "ADD unit cycles must exceed 0.";
            cin >> addCycles;
        }

        cout << "NAND unit cycles: ";
        cin >> nandCycles;
        while (nandCycles <= 0)
        {
            cout << "NAND unit cycles must exceed 0.";
            cin >> nandCycles;
        }

        cout << "DIVIDE unit cycles: ";
        cin >> divideCycles;
        while (divideCycles <= 0)
        {
            cout << "DIVIDE unit cycles must exceed 0.";
            cin >> divideCycles;
        }
    }
};

class Tomasulo
{
public:
    RegisterStat registerStat;
    FunctionalUnitManager reservationStation;
    vector<Instruction> instructionQueue;
    vector<Instruction> inflightInstructions;
    queue<Instruction> issuedQueue;
    queue<Instruction> executingQueue;
    queue<Instruction> writeBackQueue;
    array<int, 131072> Memory;
    RegisterFile registerFile;
    queue<int> LoadStoreQueue;
    map<string, bool> pleaseFree;
    queue<int> predicting;
    bool RetInFlight;
    map<int, vector<string>> instructionStatus;

    uint16_t PC;
    int ClockCycle;
    int startingAddress;

    bool Done()
    {
        for (auto it = registerStat.status.begin(); it != registerStat.status.end(); it++)
        {
            if (it->second != "")
                return false;
        }
        for (auto it = reservationStation.unitRegistry.begin(); it != reservationStation.unitRegistry.end(); it++)
        {
            if (it->second.isActive)
                return false;
        }
        return PC == instructionQueue.size();
    }

    Tomasulo()
    {
        PC = 0;
        ClockCycle = 0;
        RetInFlight = false;
        string input;
        cout << "1. Manual Instruction Entry" << '\t' << "2. Load Instructions From File" << '\n';
        cin >> input;
        // input = "1";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        while (input != "1" && input != "2")
        {
            cout << "Invalid input" << endl;
            cout << "1. Manual Instruction Entry" << '\t' << "2. Load Instructions From File";
            cin >> input;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        if (input == "1")
        {
            ReadInstructionsFromUser();
        }
        else
        {
            string filename;
            cout << "Please enter the file name: ";
            cin >> filename;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            ReadInstructionsFromFile(filename);
        }
        cout << "\n\n";
        cout << "Please enter the starting address: ";
        cin >> startingAddress;
        // startingAddress = 2000;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        initializeMem();
        for (auto it = reservationStation.unitRegistry.begin(); it != reservationStation.unitRegistry.end(); it++)
        {
            pleaseFree[it->first] = false;
        }
    }
    void RunClockCycle()
    {
        WriteBack();
        Execute();
        Issue();
        ClockCycle++;
    };
    void Issue()
    {
        if (RetInFlight)
        {
            for (auto it = pleaseFree.begin(); it != pleaseFree.end(); it++)
            {
                if (it->second)
                {
                    reservationStation.unitRegistry[it->first].operation = "";
                    reservationStation.unitRegistry[it->first].operand1 = 0;
                    reservationStation.unitRegistry[it->first].operand2 = 0;
                    reservationStation.unitRegistry[it->first].dependency1 = "";
                    reservationStation.unitRegistry[it->first].dependency2 = "";
                    reservationStation.unitRegistry[it->first].address = 0;
                    reservationStation.unitRegistry[it->first].isActive = false;
                    reservationStation.unitRegistry[it->first].startCycle = 0;
                    reservationStation.unitRegistry[it->first].executionEndCycle = 0;
                    reservationStation.unitRegistry[it->first].resultValue = 0;
                    reservationStation.unitRegistry[it->first].isCompleted = false;
                    it->second = false;
                    if (it->first[0] == 'C' || it->first[0] == 'R')
                        reservationStation.activeCallReturnUnits--;
                    else if (it->first[0] == 'A')
                        reservationStation.activeAddUnits--;
                    else if (it->first[0] == 'L')
                        reservationStation.activeLoadUnits--;
                    else if (it->first[0] == 'D')
                        reservationStation.activeDivideUnits--;
                    else if (it->first[0] == 'N')
                        reservationStation.activeNandUnits--;
                    else if (it->first[0] == 'S')
                        reservationStation.activeStoreUnits--;
                    else if (it->first[0] == 'B')
                        reservationStation.activeBranchUnits--;
                }
            }
            return;
        }
        if (PC >= instructionQueue.size())
        {
            for (auto it = pleaseFree.begin(); it != pleaseFree.end(); it++)
            {
                if (it->second)
                {
                    reservationStation.unitRegistry[it->first].operation = "";
                    reservationStation.unitRegistry[it->first].operand1 = 0;
                    reservationStation.unitRegistry[it->first].operand2 = 0;
                    reservationStation.unitRegistry[it->first].dependency1 = "";
                    reservationStation.unitRegistry[it->first].dependency2 = "";
                    reservationStation.unitRegistry[it->first].address = 0;
                    reservationStation.unitRegistry[it->first].isActive = false;
                    reservationStation.unitRegistry[it->first].startCycle = 0;
                    reservationStation.unitRegistry[it->first].executionEndCycle = 0;
                    reservationStation.unitRegistry[it->first].resultValue = 0;
                    reservationStation.unitRegistry[it->first].isCompleted = false;
                    it->second = false;
                    if (it->first[0] == 'C' || it->first[0] == 'R')
                        reservationStation.activeCallReturnUnits--;
                    else if (it->first[0] == 'A')
                        reservationStation.activeAddUnits--;
                    else if (it->first[0] == 'L')
                        reservationStation.activeLoadUnits--;
                    else if (it->first[0] == 'D')
                        reservationStation.activeDivideUnits--;
                    else if (it->first[0] == 'N')
                        reservationStation.activeNandUnits--;
                    else if (it->first[0] == 'S')
                        reservationStation.activeStoreUnits--;
                    else if (it->first[0] == 'B')
                        reservationStation.activeBranchUnits--;
                }
            }
            return;
        }
        Instruction instruction = instructionQueue[PC];
        string currentStation;
        if (instruction.op == "LOAD")
        {
            if (reservationStation.activeLoadUnits < reservationStation.loadUnits)
            {
                for (int i = 0; i < reservationStation.loadUnits; i++)
                {
                    currentStation = "LoadUnit" + to_string(i);
                    if (!reservationStation.unitRegistry[currentStation].isActive)
                    {
                        break;
                    }
                }
                instructionStatus[ClockCycle].push_back(instruction.Inst);
                instructionStatus[ClockCycle].push_back(to_string(ClockCycle));
                if (registerStat.status[instruction.RS1] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency1 = registerStat.status[instruction.RS1];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand1 = registerFile[(instruction.RS1[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency1 = "";
                }

                reservationStation.unitRegistry[currentStation].address = (instruction.offset);
                reservationStation.unitRegistry[currentStation].isActive = true;

                reservationStation.unitRegistry[currentStation].instructionStart = PC;
                reservationStation.unitRegistry[currentStation].startCycle = ClockCycle;
                reservationStation.unitRegistry[currentStation].operation = instruction.op;

                registerStat.status[instruction.RD] = currentStation;

                inflightInstructions.push_back(instruction);
                LoadStoreQueue.push(ClockCycle);
                reservationStation.activeLoadUnits++;
                issuedQueue.push(instruction);
                PC++;
            }
        }
        else if (instruction.op == "STORE")
        {
            if (reservationStation.activeStoreUnits < reservationStation.storeUnits)
            {
                for (int i = 0; i < reservationStation.storeUnits; i++)
                {
                    currentStation = "StoreUnit" + to_string(i);
                    if (!reservationStation.unitRegistry[currentStation].isActive)
                    {
                        break;
                    }
                }
                instructionStatus[ClockCycle].push_back(instruction.Inst);
                instructionStatus[ClockCycle].push_back(to_string(ClockCycle));
                if (registerStat.status[instruction.RS1] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency1 = registerStat.status[instruction.RS1];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand1 = registerFile[(instruction.RS1[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency1 = "";
                }

                if (registerStat.status[instruction.RS2] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency2 = registerStat.status[instruction.RS2];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand2 = registerFile[(instruction.RS2[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency2 = "";
                }

                reservationStation.unitRegistry[currentStation].address = (instruction.offset);
                reservationStation.unitRegistry[currentStation].isActive = true;

                reservationStation.unitRegistry[currentStation].instructionStart = PC;
                reservationStation.unitRegistry[currentStation].startCycle = ClockCycle;
                reservationStation.unitRegistry[currentStation].operation = instruction.op;

                inflightInstructions.push_back(instruction);
                LoadStoreQueue.push(ClockCycle);
                reservationStation.activeStoreUnits++;
                issuedQueue.push(instruction);
                PC++;
            }
        }
        else if (instruction.op == "BNE")
        {
            if (reservationStation.activeBranchUnits < reservationStation.branchUnits)
            {
                for (int i = 0; i < reservationStation.branchUnits; i++)
                {
                    currentStation = "BranchUnit" + to_string(i);
                    if (!reservationStation.unitRegistry[currentStation].isActive)
                    {
                        break;
                    }
                }
                instructionStatus[ClockCycle].push_back(instruction.Inst);
                instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                if (registerStat.status[instruction.RS1] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency1 = registerStat.status[instruction.RS1];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand1 = registerFile[(instruction.RS1[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency1 = "";
                }

                if (registerStat.status[instruction.RS2] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency2 = registerStat.status[instruction.RS2];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand2 = registerFile[(instruction.RS2[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency2 = "";
                }

                reservationStation.unitRegistry[currentStation].address = (instruction.label);
                reservationStation.unitRegistry[currentStation].isActive = true;

                reservationStation.unitRegistry[currentStation].instructionStart = PC;
                reservationStation.unitRegistry[currentStation].startCycle = ClockCycle;
                reservationStation.unitRegistry[currentStation].operation = instruction.op;

                inflightInstructions.push_back(instruction);
                reservationStation.activeBranchUnits++;
                issuedQueue.push(instruction);

                predicting.push(ClockCycle);
                PC++;
            }
        }
        else if (instruction.op == "CALL")
        {
            if (reservationStation.activeCallReturnUnits < reservationStation.callReturnUnits)
            {
                if (predicting.empty() || predicting.front() > ClockCycle)
                {

                    for (int i = 0; i < reservationStation.callReturnUnits; i++)
                    {
                        currentStation = "CallReturnUnit" + to_string(i);
                        if (!reservationStation.unitRegistry[currentStation].isActive)
                        {
                            break;
                        }
                    }

                    instructionStatus[ClockCycle].push_back(instruction.Inst);
                    instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                    reservationStation.unitRegistry[currentStation].address = (instruction.label);
                    registerStat.status["R1"] = currentStation;
                    reservationStation.unitRegistry[currentStation].isActive = true;

                    reservationStation.unitRegistry[currentStation].instructionStart = PC;
                    reservationStation.unitRegistry[currentStation].startCycle = ClockCycle;
                    reservationStation.unitRegistry[currentStation].operation = instruction.op;
                    PC = instruction.label - startingAddress;

                    inflightInstructions.push_back(instruction);
                    reservationStation.activeCallReturnUnits++;
                    issuedQueue.push(instruction);
                }
            }
        }
        else if (instruction.op == "RET")
        {
            if (reservationStation.activeCallReturnUnits < reservationStation.callReturnUnits)
            {
                for (int i = 0; i < reservationStation.callReturnUnits; i++)
                {
                    currentStation = "CallReturnUnit" + to_string(i);
                    if (!reservationStation.unitRegistry[currentStation].isActive)
                    {
                        break;
                    }
                }
                instructionStatus[ClockCycle].push_back(instruction.Inst);
                instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                reservationStation.unitRegistry[currentStation].isActive = true;

                reservationStation.unitRegistry[currentStation].instructionStart = PC;
                reservationStation.unitRegistry[currentStation].startCycle = ClockCycle;
                reservationStation.unitRegistry[currentStation].operation = instruction.op;

                inflightInstructions.push_back(instruction);
                reservationStation.activeCallReturnUnits++;
                RetInFlight = true;
                issuedQueue.push(instruction);
            }
        }
        else if (instruction.op == "ADD")
        {
            if (reservationStation.activeAddUnits < reservationStation.addUnits)
            {
                for (int i = 0; i < reservationStation.addUnits; i++)
                {
                    currentStation = "AddUnit" + to_string(i);
                    if (!reservationStation.unitRegistry[currentStation].isActive)
                    {
                        break;
                    }
                }
                instructionStatus[ClockCycle].push_back(instruction.Inst);
                instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                if (registerStat.status[instruction.RS1] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency1 = registerStat.status[instruction.RS1];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand1 = registerFile[(instruction.RS1[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency1 = "";
                }

                if (registerStat.status[instruction.RS2] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency2 = registerStat.status[instruction.RS2];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand2 = registerFile[(instruction.RS2[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency2 = "";
                }

                reservationStation.unitRegistry[currentStation].isActive = true;

                reservationStation.unitRegistry[currentStation].instructionStart = PC;
                reservationStation.unitRegistry[currentStation].startCycle = ClockCycle;
                reservationStation.unitRegistry[currentStation].operation = instruction.op;

                registerStat.status[instruction.RD] = currentStation;

                inflightInstructions.push_back(instruction);
                reservationStation.activeAddUnits++;
                issuedQueue.push(instruction);

                PC++;
            }
        }
        else if (instruction.op == "ADDI")
        {
            if (reservationStation.activeAddUnits < reservationStation.addUnits)
            {
                for (int i = 0; i < reservationStation.addUnits; i++)
                {
                    currentStation = "AddUnit" + to_string(i);
                    if (!reservationStation.unitRegistry[currentStation].isActive)
                    {
                        break;
                    }
                }
                instructionStatus[ClockCycle].push_back(instruction.Inst);
                instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                if (registerStat.status[instruction.RS1] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency1 = registerStat.status[instruction.RS1];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand1 = registerFile[(instruction.RS1[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency1 = "";
                }

                reservationStation.unitRegistry[currentStation].operand2 = instruction.IMM;
                reservationStation.unitRegistry[currentStation].dependency2 = "";

                reservationStation.unitRegistry[currentStation].instructionStart = PC;
                reservationStation.unitRegistry[currentStation].startCycle = ClockCycle;
                reservationStation.unitRegistry[currentStation].operation = instruction.op;

                reservationStation.unitRegistry[currentStation].isActive = true;
                registerStat.status[instruction.RD] = currentStation;

                inflightInstructions.push_back(instruction);
                reservationStation.activeAddUnits++;
                issuedQueue.push(instruction);

                PC++;
            }
        }
        else if (instruction.op == "NAND")
        {
            if (reservationStation.activeNandUnits < reservationStation.nandUnits)
            {
                for (int i = 0; i < reservationStation.nandUnits; i++)
                {
                    currentStation = "NandUnit" + to_string(i);
                    if (!reservationStation.unitRegistry[currentStation].isActive)
                    {
                        break;
                    }
                }

                instructionStatus[ClockCycle].push_back(instruction.Inst);
                instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                if (registerStat.status[instruction.RS1] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency1 = registerStat.status[instruction.RS1];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand1 = registerFile[(instruction.RS1[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency1 = "";
                }

                if (registerStat.status[instruction.RS2] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency2 = registerStat.status[instruction.RS2];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand2 = registerFile[(instruction.RS2[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency2 = "";
                }

                reservationStation.unitRegistry[currentStation].isActive = true;
                registerStat.status[instruction.RD] = currentStation;

                reservationStation.unitRegistry[currentStation].instructionStart = PC;
                reservationStation.unitRegistry[currentStation].startCycle = ClockCycle;
                reservationStation.unitRegistry[currentStation].operation = instruction.op;

                inflightInstructions.push_back(instruction);
                reservationStation.activeNandUnits++;
                issuedQueue.push(instruction);
                PC++;
            }
        }
        else if (instruction.op == "DIV")
        {
            if (reservationStation.activeDivideUnits < reservationStation.divideUnits)
            {
                for (int i = 0; i < reservationStation.divideUnits; i++)
                {
                    currentStation = "DivideUnit" + to_string(i);
                    if (!reservationStation.unitRegistry[currentStation].isActive)
                    {
                        break;
                    }
                }

                instructionStatus[ClockCycle].push_back(instruction.Inst);
                instructionStatus[ClockCycle].push_back(to_string(ClockCycle));

                if (registerStat.status[instruction.RS1] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency1 = registerStat.status[instruction.RS1];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand1 = registerFile[(instruction.RS1[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency1 = "";
                }

                if (registerStat.status[instruction.RS2] != "")
                {
                    reservationStation.unitRegistry[currentStation].dependency2 = registerStat.status[instruction.RS2];
                }
                else
                {
                    reservationStation.unitRegistry[currentStation].operand2 = registerFile[(instruction.RS2[1] - '0')];
                    reservationStation.unitRegistry[currentStation].dependency2 = "";
                }

                reservationStation.unitRegistry[currentStation].isActive = true;

                reservationStation.unitRegistry[currentStation].instructionStart = PC;
                reservationStation.unitRegistry[currentStation].startCycle = ClockCycle;
                reservationStation.unitRegistry[currentStation].operation = instruction.op;

                registerStat.status[instruction.RD] = currentStation;

                inflightInstructions.push_back(instruction);
                reservationStation.activeDivideUnits++;
                issuedQueue.push(instruction);
                PC++;
            }
        }
        else
        {
            cout << "Invalid instruction ISSUE: " << instruction.Inst << endl;
            return;
        }

        for (auto it = pleaseFree.begin(); it != pleaseFree.end(); it++)
        {
            if (it->second)
            {
                reservationStation.unitRegistry[it->first].operation = "";
                reservationStation.unitRegistry[it->first].operand1 = 0;
                reservationStation.unitRegistry[it->first].operand2 = 0;
                reservationStation.unitRegistry[it->first].dependency1 = "";
                reservationStation.unitRegistry[it->first].dependency2 = "";
                reservationStation.unitRegistry[it->first].address = 0;
                reservationStation.unitRegistry[it->first].isActive = false;
                reservationStation.unitRegistry[it->first].startCycle = 0;
                reservationStation.unitRegistry[it->first].executionEndCycle = 0;
                reservationStation.unitRegistry[it->first].resultValue = 0;
                reservationStation.unitRegistry[it->first].isCompleted = false;
                it->second = false;
                if (it->first[0] == 'C' || it->first[0] == 'R')
                    reservationStation.activeCallReturnUnits--;
                else if (it->first[0] == 'A')
                    reservationStation.activeAddUnits--;
                else if (it->first[0] == 'L')
                    reservationStation.activeLoadUnits--;
                else if (it->first[0] == 'D')
                    reservationStation.activeDivideUnits--;
                else if (it->first[0] == 'N')
                    reservationStation.activeNandUnits--;
                else if (it->first[0] == 'S')
                    reservationStation.activeStoreUnits--;
                else if (it->first[0] == 'B')
                    reservationStation.activeBranchUnits--;
            }
        }
    };
    void Execute()
    {
        if (RetInFlight)
        {
            auto it = reservationStation.unitRegistry.begin();
            while (it != reservationStation.unitRegistry.end())
            {
                if (it->second.isActive && !it->second.isCompleted && ((predicting.empty()) || !(predicting.front() < it->second.startCycle)) && it->second.operation == "RET")
                {
                    it->second.executionEndCycle = ClockCycle + reservationStation.callReturnCycles;
                    instructionStatus[it->second.startCycle].push_back(to_string(ClockCycle));
                    instructionStatus[it->second.startCycle].push_back(to_string(it->second.executionEndCycle));
                    it->second.isCompleted = true;
                }
                it++;
            }

            return;
        }
        auto it = reservationStation.unitRegistry.begin();
        bool popLoadStore = false;
        while (it != reservationStation.unitRegistry.end())
        {
            if (it->second.isActive && !it->second.isCompleted && ((predicting.empty()) || !(predicting.front() < it->second.startCycle)))
            {
                if (it->second.operation == "LOAD")
                {
                    if (it->second.dependency1 == "" && it->second.startCycle == LoadStoreQueue.front())
                    {
                        it->second.resultValue = Memory[it->second.operand1 + (it->second.address)];
                        popLoadStore = true;
                        it->second.executionEndCycle = ClockCycle + reservationStation.loadCycles;
                        instructionStatus[it->second.startCycle].push_back(to_string(ClockCycle));
                        instructionStatus[it->second.startCycle].push_back(to_string(it->second.executionEndCycle));
                        it->second.isCompleted = true;
                    }
                }
                else if (it->second.operation == "STORE")
                {
                    if (it->second.dependency1 == "" && it->second.startCycle == LoadStoreQueue.front())
                    {
                        it->second.address = it->second.operand1 + (it->second.address);
                        popLoadStore = true;
                        it->second.executionEndCycle = ClockCycle + reservationStation.storeCycles;
                        instructionStatus[it->second.startCycle].push_back(to_string(ClockCycle));
                        instructionStatus[it->second.startCycle].push_back(to_string(it->second.executionEndCycle));
                        it->second.isCompleted = true;
                    }
                }
                else if (it->second.operation == "BNE")
                {
                    if (it->second.dependency1 == "" && it->second.dependency2 == "")
                    {
                        it->second.resultValue = it->second.operand1 != it->second.operand2;
                        it->second.executionEndCycle = ClockCycle + reservationStation.branchCycles;
                        instructionStatus[it->second.startCycle].push_back(to_string(ClockCycle));
                        instructionStatus[it->second.startCycle].push_back(to_string(it->second.executionEndCycle));
                        it->second.isCompleted = true;
                    }
                }
                else if (it->second.operation == "CALL")
                {
                    if (it->second.dependency1 == "" && it->second.dependency2 == "")
                    {
                        it->second.resultValue = it->second.instructionStart + 1;
                        it->second.executionEndCycle = ClockCycle + reservationStation.callReturnCycles - 1;
                        instructionStatus[it->second.startCycle].push_back(to_string(ClockCycle));
                        instructionStatus[it->second.startCycle].push_back(to_string(it->second.executionEndCycle));
                        it->second.isCompleted = true;
                    }
                }
                else if (it->second.operation == "RET")
                {
                    it->second.executionEndCycle = ClockCycle + reservationStation.callReturnCycles;
                    instructionStatus[it->second.startCycle].push_back(to_string(ClockCycle));
                    instructionStatus[it->second.startCycle].push_back(to_string(it->second.executionEndCycle));
                    it->second.isCompleted = true;
                }
                else if (it->second.operation == "ADD")
                {
                    if (it->second.dependency1 == "" && it->second.dependency2 == "")
                    {
                        it->second.resultValue = it->second.operand1 + it->second.operand2;
                        it->second.executionEndCycle = ClockCycle + reservationStation.addCycles - 1;
                        instructionStatus[it->second.startCycle].push_back(to_string(ClockCycle));
                        instructionStatus[it->second.startCycle].push_back(to_string(it->second.executionEndCycle));
                        it->second.isCompleted = true;
                    }
                }
                else if (it->second.operation == "ADDI")
                {
                    if (it->second.dependency1 == "")
                    {
                        it->second.resultValue = it->second.operand1 + it->second.operand2;
                        it->second.executionEndCycle = ClockCycle + reservationStation.addCycles - 1;
                        instructionStatus[it->second.startCycle].push_back(to_string(ClockCycle));
                        instructionStatus[it->second.startCycle].push_back(to_string(it->second.executionEndCycle));
                        it->second.isCompleted = true;
                    }
                }
                else if (it->second.operation == "NAND")
                {
                    if (it->second.dependency1 == "" && it->second.dependency2 == "")
                    {
                        it->second.resultValue = ~(it->second.operand1 & it->second.operand2);
                        it->second.executionEndCycle = ClockCycle + reservationStation.nandCycles - 1;
                        instructionStatus[it->second.startCycle].push_back(to_string(ClockCycle));
                        instructionStatus[it->second.startCycle].push_back(to_string(it->second.executionEndCycle));
                        it->second.isCompleted = true;
                    }
                }
                else if (it->second.operation == "DIV")
                {
                    if (it->second.dependency1 == "" && it->second.dependency2 == "")
                    {
                        it->second.resultValue = it->second.operand1 / it->second.operand2;
                        it->second.executionEndCycle = ClockCycle + reservationStation.divideCycles - 1;
                        instructionStatus[it->second.startCycle].push_back(to_string(ClockCycle));
                        instructionStatus[it->second.startCycle].push_back(to_string(it->second.executionEndCycle));
                        it->second.isCompleted = true;
                    }
                }
                else
                {
                    cout << "Invalid instruction EXEC: " << it->second.operation << endl;
                }
            }
            it++;
        }

        if (popLoadStore)
        {
            LoadStoreQueue.pop();
            popLoadStore = false;
        }
    }
    class myCompare
    {
    public:
        int operator()(const UnitEntry &lhs, const UnitEntry &rhs) const
        {
            return lhs.startCycle > rhs.startCycle;
        }
    };
    void WriteBack()
    {
        priority_queue<UnitEntry, vector<UnitEntry>, myCompare> pq;

        for (auto it = reservationStation.unitRegistry.begin(); it != reservationStation.unitRegistry.end(); it++)
        {
            if (it->second.isActive && it->second.isCompleted && it->second.executionEndCycle < ClockCycle)
                pq.push(it->second);
        }
        if (pq.empty())
        {
            return;
        }
        // cout<<"PQ.TOP(): " << pq.top().unitID[0] << endl;
        if (pq.top().unitID[0] == 'A' || pq.top().unitID[0] == 'L' || pq.top().unitID[0] == 'D' || pq.top().unitID[0] == 'N')
        {
            if (pq.top().executionEndCycle < ClockCycle)
            {
                instructionStatus[pq.top().startCycle].push_back(to_string(ClockCycle));
                for (int i = 0; i < 8; i++)
                {
                    if (registerStat.status["R" + to_string(i)] == pq.top().unitID)
                    {
                        registerFile[i] = pq.top().resultValue;
                        registerStat.status["R" + to_string(i)] = "";
                    }
                }
                for (auto it = reservationStation.unitRegistry.begin(); it != reservationStation.unitRegistry.end(); it++)
                {
                    if (it->second.dependency1 == pq.top().unitID)
                    {
                        it->second.operand1 = pq.top().resultValue;
                        it->second.dependency1 = "";
                    }
                    if (it->second.dependency2 == pq.top().unitID)
                    {
                        it->second.operand2 = pq.top().resultValue;
                        it->second.dependency2 = "";
                    }
                }
                if (pq.top().unitID[0] == 'A')
                {
                    pleaseFree[pq.top().unitID] = true;
                }
                else if (pq.top().unitID[0] == 'L')
                {
                    pleaseFree[pq.top().unitID] = true;
                }
                else if (pq.top().unitID[0] == 'D')
                {
                    pleaseFree[pq.top().unitID] = true;
                }
                else if (pq.top().unitID[0] == 'N')
                {
                    pleaseFree[pq.top().unitID] = true;
                }
            }
        }
        else if (pq.top().unitID[0] == 'S' && pq.top().executionEndCycle < ClockCycle)
        {
            if (pq.top().dependency2 == "")
            {
                Memory[pq.top().address] = pq.top().operand2;
                pleaseFree[pq.top().unitID] = true;
                instructionStatus[pq.top().startCycle].push_back(to_string(ClockCycle));
            }
        }
        else if (pq.top().unitID[0] == 'B' && pq.top().executionEndCycle < ClockCycle)
        {
            if (!predicting.empty())
                predicting.pop();
            if (pq.top().resultValue)
            {
                PC = pq.top().address + pq.top().instructionStart;
                instructionStatus[pq.top().startCycle].push_back(to_string(ClockCycle));
                pleaseFree[pq.top().unitID] = true;

                for (auto it = reservationStation.unitRegistry.begin(); it != reservationStation.unitRegistry.end(); it++)
                {
                    if (it->second.startCycle > pq.top().startCycle)
                    {
                        if (it->second.unitID[0] == 'B')
                        {
                            if (!predicting.empty())
                                predicting.pop();
                        }
                        else if (it->second.unitID[0] == 'L' || it->second.unitID[0] == 'S')
                        {
                            if (!LoadStoreQueue.empty())
                                LoadStoreQueue.pop();
                        }
                        it->second.operation = "";
                        it->second.operand1 = 0;
                        it->second.operand2 = 0;
                        it->second.dependency1 = "";
                        it->second.dependency2 = "";
                        it->second.address = 0;
                        it->second.isActive = false;
                        it->second.startCycle = 0;
                        it->second.executionEndCycle = 0;
                        it->second.resultValue = 0;
                        if (it->first[0] == 'C' || it->first[0] == 'R')
                            reservationStation.activeCallReturnUnits--;
                        else if (it->first[0] == 'A')
                            reservationStation.activeAddUnits--;
                        else if (it->first[0] == 'L')
                            reservationStation.activeLoadUnits--;
                        else if (it->first[0] == 'D')
                            reservationStation.activeDivideUnits--;
                        else if (it->first[0] == 'N')
                            reservationStation.activeNandUnits--;
                        else if (it->first[0] == 'S')
                            reservationStation.activeStoreUnits--;
                        else if (it->first[0] == 'B')
                            reservationStation.activeBranchUnits--;
                    }
                }
            }
            else
            {
                pleaseFree[pq.top().unitID] = true;
                instructionStatus[pq.top().startCycle].push_back(to_string(ClockCycle));
            }
        }
        else if (pq.top().unitID[0] == 'C' && pq.top().executionEndCycle < ClockCycle)
        {
            if (pq.top().operation == "CALL")
            {
                if (registerStat.status["R1"] == pq.top().unitID)
                {
                    registerFile[1] = pq.top().resultValue;
                    registerStat.status["R1"] = "";
                }
                pleaseFree[pq.top().unitID] = true;
                instructionStatus[pq.top().startCycle].push_back(to_string(ClockCycle));
            }
            else
            {
                if (registerStat.status["R1"] == "")
                {
                    RetInFlight = false;
                    PC = registerFile[1];
                    pleaseFree[pq.top().unitID] = true;
                    instructionStatus[pq.top().startCycle].push_back(to_string(ClockCycle));
                }
            }
        }
        else if (pq.top().unitID[0] == 'R' && pq.top().executionEndCycle < ClockCycle)
        {
            if (registerStat.status["R1"] == "")
            {
                RetInFlight = false;
                PC = registerFile[1];
                pleaseFree[pq.top().unitID] = true;
                instructionStatus[pq.top().startCycle].push_back(to_string(ClockCycle));
            }
        }
        else
        {
            cout << "Invalid instruction: " << pq.top().unitID << " " << pq.top().operation << endl;
            pq.pop();
            return;
        }
    }
    void initializeMem()
    {
        cout << "\n\n----------------------------------------------------------------------------------------\n\n";
        cout << "Enter values for memory" << endl;
        cout << "Enter -1 to stop entering values" << endl
             << endl;
        int address = 0;
        while (address != -1)
        {
            cout << "Enter address: ";
            cin >> address;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            while (address < -1 || address > 131072)
            {
                cout << "Address must be between 0 and 131072" << endl;
                cout << "Enter address: ";
                cin >> address;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            if (address != -1)
            {
                cout << "Enter value: ";
                cin >> Memory[address];
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cout << endl;
        }
        cout << "\n\n----------------------------------------------------------------------------------------\n\n";
    }

    void ReadInstructionsFromFile(string filename)
    {
        ifstream file(filename);
        while (!file.is_open())
        {
            cout << "File does not exist" << endl;
            cout << "Please enter the file path relative to this file:: ";
            cin >> filename;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            file.open(filename);
        }
        string line;
        while (getline(file, line))
        {
            instructionQueue.push_back(Instruction(line));
        }
        file.close();
    }
    void ReadInstructionsFromUser()
    {
        string input;
        cout << "Please enter instructions (enter done to stop): " << endl;
        while (input != "done")
        {
            getline(cin, input);
            // cin.ignore('\n');
            if (input != "done")
            {
                instructionQueue.push_back(Instruction(input));
            }
        }
        for (int i = 0; i < instructionQueue.size(); i++)
        {
            cout << instructionQueue[i].Inst << '\t';
            cout << instructionQueue[i].op << endl;
        }
    }
    void printTable()
    {
        cout << "Cycle \t" << "Instruction \t\t" << "Issue \t" << "Start EX \t" << "End EX\t\t" << "WB\t" << endl;
        for (auto it = instructionStatus.begin(); it != instructionStatus.end(); it++)
        {
            if (it->second.size() == 5)
            {
                if (it->second[0][0] == 'R' || it->second[0][0] == 'C')
                {
                    cout << it->first << "\t" << it->second[0] << "\t\t\t" << it->second[1] << "\t" << it->second[2] << "\t\t" << it->second[3] << "\t\t" << it->second[4] << endl;
                }
                else
                {
                    cout << it->first << "\t" << it->second[0] << "\t\t" << it->second[1] << "\t" << it->second[2] << "\t\t" << it->second[3] << "\t\t" << it->second[4] << endl;
                }
            }
            else if (it->second.size() == 2)
            {
                cout << it->first << "\t" << it->second[0] << "\t\t" << it->second[1] << "\tFLUSHED" << endl;
            }
        }
    }
};

int main()
{

    Tomasulo tomasulo;

    while (!tomasulo.Done())
    {
        tomasulo.RunClockCycle();
    }
    tomasulo.printTable();
    cout << "Last write back at cycle: " << tomasulo.ClockCycle - 1 << " CPU was empty at cycle: " << tomasulo.ClockCycle << endl;
    return 0;
}