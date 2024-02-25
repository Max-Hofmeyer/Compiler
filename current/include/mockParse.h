#pragma once

class MockParser : public SubscriberInterface {
public:
	void Update(const token& t) override {
		//std::cout << "Got the token: " << tk.typeString << ", line: " << std::to_string(tk.lineLoc) << "\n";
		Logger::parser(t.typeString + " at line " + std::to_string(t.lineLoc));
		_tokenLine.emplace_back(t);
	}

private:
	std::vector<token> _tokenLine;
};