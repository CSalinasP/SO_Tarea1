vector<string> parseLine(const string &line, char delimiter = ' ');
void executeNormalCommand(vector<string> &args);
void executePipelineCommand(vector<vector<string>> &commands);
void executeMiprof(vector<string> command);
void executeMiprofMaxTime(vector<string> command, int maxTime);
void executeMiprofEjecsave(vector<string> command, char* archive);