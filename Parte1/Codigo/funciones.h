vector<string> parseLine(const string &line, char delimiter = ' ');
void executeNormalCommand(vector<string> &args);
void executePipelineCommand(vector<vector<string>> &commands);