//#include "toCPP.hpp"
//
//struct scoped_transpilation_state {
//	variables_t& variables;
//	std::vector<std::string> temp_variables;
//
//	Named& named;
//	std::vector<std::string> temp_functions;
//	std::vector<std::string> temp_function_templates;
//	std::vector<std::string> temp_types;
//	std::vector<std::string> temp_type_templates;
//	std::vector<std::string> temp_blocks;
//	std::vector<std::string> temp_block_templates;
//
//	~scoped_transpilation_state() {
//		for (const auto& s : temp_variables)
//								  variables[s].pop_back();
//		for (const auto& s : temp_functions)
//			named.				  functions[s].pop_back();
//		for (const auto& s : temp_function_templates)
//			named.				  function_templates[s].pop_back();
//		for (const auto& s : temp_types)
//			named.				  types[s].pop_back();
//		for (const auto& s : temp_type_templates)
//			named.				  type_templates[s].pop_back();
//		for (const auto& s : temp_blocks)
//			named.				  blocks[s].pop_back();
//		for (const auto& s : temp_block_templates)
//			named.				  block_templates[s].pop_back();
//	}
//};
//
//void move_combine_scoped_states(scoped_transpilation_state& destination, scoped_transpilation_state&& source) {
//	if (&destination.named != &source.named || &destination.variables != &source.variables)
//		throw std::runtime_error("internal compiler error");
//
//	destination.temp_variables.insert(destination.temp_variables.end(), std::make_move_iterator(source.temp_variables.begin()), std::make_move_iterator(source.temp_variables.end()));
//	destination.temp_functions.insert(destination.temp_functions.end(), std::make_move_iterator(source.temp_functions.begin()), std::make_move_iterator(source.temp_functions.end()));
//	destination.temp_function_templates.insert(destination.temp_function_templates.end(), std::make_move_iterator(source.temp_function_templates.begin()), std::make_move_iterator(source.temp_function_templates.end()));
//	destination.temp_types.insert(destination.temp_types.end(), std::make_move_iterator(source.temp_types.begin()), std::make_move_iterator(source.temp_types.end()));
//	destination.temp_type_templates.insert(destination.temp_type_templates.end(), std::make_move_iterator(source.temp_type_templates.begin()), std::make_move_iterator(source.temp_type_templates.end()));
//	destination.temp_blocks.insert(destination.temp_blocks.end(), std::make_move_iterator(source.temp_blocks.begin()), std::make_move_iterator(source.temp_blocks.end()));
//	destination.temp_block_templates.insert(destination.temp_block_templates.end(), std::make_move_iterator(source.temp_block_templates.begin()), std::make_move_iterator(source.temp_block_templates.end()));
//}
//
//void add(scoped_transpilation_state& state, const std::string& name, std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>&& variable) {
//	state.temp_variables.push_back(name);
//	state.variables[name].push_back(std::move(variable));
//}
//
//void add(scoped_transpilation_state& state, const std::string& name, const NodeStructs::Function& x) {
//	state.temp_functions.push_back(name);
//	state.named.functions[name].push_back(x);
//}
//
//void add(scoped_transpilation_state& state, const std::string& name, const NodeStructs::Template<NodeStructs::Function>& x) {
//	state.temp_function_templates.push_back(name);
//	state.named.function_templates[name].push_back(x);
//}
//
//void add(scoped_transpilation_state& state, const std::string& name, const NodeStructs::Type& x) {
//	state.temp_types.push_back(name);
//	state.named.types[name].push_back(x);
//}
//
//void add(scoped_transpilation_state& state, const std::string& name, const NodeStructs::Template<NodeStructs::Type>& x) {
//	state.temp_type_templates.push_back(name);
//	state.named.type_templates[name].push_back(x);
//}
//
//void add(scoped_transpilation_state& state, const std::string& name, const NodeStructs::Block& x) {
//	state.temp_blocks.push_back(name);
//	state.named.blocks[name].push_back(x);
//}
//
//void add(scoped_transpilation_state& state, const std::string& name, const NodeStructs::Template<NodeStructs::Block>& x) {
//	state.temp_block_templates.push_back(name);
//	state.named.block_templates[name].push_back(x);
//}
