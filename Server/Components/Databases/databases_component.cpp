#include "databases_component.hpp"

DatabasesComponent databaseComponent;

DatabasesComponent::DatabasesComponent() {
	databaseConnections.claimUnusable(0);
	databaseResultSets.claimUnusable(0);
}

/// Creates a  result set
/// @returns Result set if successful, otherwise "nullptr"
IDatabaseResultSet* DatabasesComponent::createResultSet() {
	IDatabaseResultSet* ret(nullptr);
	int result_set_index(databaseResultSets.claim());
	/// TODO: Handle invalid indices properly
	if (result_set_index >= 0) {
		ret = &databaseResultSets.get(result_set_index);
	}
	return ret;
}

/// Gets the component name
/// @returns Component name
StringView DatabasesComponent::componentName() {
	return "Databases";
}

/// Gets the component type
/// @returns Component type
ComponentType DatabasesComponent::componentType() {
	return ComponentType::Other;
}

/// Called for every component after components have been loaded
/// Should be used for storing the core interface, registering player/core event handlers
/// Should NOT be used for interacting with other components as they might not have been initialised yet
void DatabasesComponent::onLoad(ICore* c) {}

/// Opens a new database connection
/// @param path Path to the database
/// @returns Database if successful, otherwise "nullptr"
IDatabaseConnection* DatabasesComponent::open(StringView path) {
	DatabaseConnection* ret(nullptr);
	int database_connection_index(databaseConnections.findFreeIndex());
	// TODO: Properly handle invalid indices
	if (database_connection_index >= 0) {
		sqlite3* database_connection_handle(nullptr);
		// TODO: Properly handle errors
		if (sqlite3_open_v2(path.data(), &database_connection_handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) == SQLITE_OK) {
			database_connection_index = databaseConnections.claim(database_connection_index);
			if (database_connection_index < 0) {
				sqlite3_close_v2(database_connection_handle);
				ret = nullptr;
			}
			else {
				ret = &databaseConnections.get(database_connection_index);
				ret->parentDatabasesComponent = this;
				ret->databaseConnectionHandle = database_connection_handle;
			}
		}
	}
	return ret;
}

/// Closes the specified database connection
/// @param databaseConnection Database connection
/// @returns "true" if database connection has been successfully closed, otherwise "false"
bool DatabasesComponent::close(IDatabaseConnection& connection) {
	int database_connection_index(connection.getID());
	bool ret(databaseConnections.valid(database_connection_index));
	if (ret) {
		databaseConnections.get(database_connection_index).close();
		databaseConnections.remove(database_connection_index);
	}
	return ret;
}

/// Frees the specified result set
/// @param resultSet Result set
/// @returns "true" if result set has been successfully freed, otherwise "false"
bool DatabasesComponent::freeResultSet(IDatabaseResultSet& resultSet) {
	int database_result_set_index(resultSet.getID());
	bool ret(databaseResultSets.valid(database_result_set_index));
	if (ret) {
		databaseResultSets.remove(database_result_set_index);
	}
	return ret;
}

/// Gets the number of database connections
/// @returns Number of database connections
std::size_t DatabasesComponent::getDatabaseConnectionCount() const {
	return databaseConnections.entries().size();
}

/// Is database connection ID valid
/// @param databaseConnectionID Database connection ID
/// @returns "true" if database connection ID is valid, otherwise "false"
bool DatabasesComponent::isDatabaseConnectionIDValid(int databaseConnectionID) const {
	return databaseConnections.valid(databaseConnectionID);
}

/// Gets a database connection by ID
/// @param databaseConnectionID Database connection ID
/// @returns Database connection
IDatabaseConnection& DatabasesComponent::getDatabaseConnectionByID(int databaseConnectionID) {
	return databaseConnections.get(databaseConnectionID);
}

/// Gets the number of database result sets
/// @returns Number of result sets
std::size_t DatabasesComponent::getDatabaseResultSetCount() const {
	return databaseResultSets.entries().size();
}

/// Is database result set ID valid
/// @param databaseResultSetID Database result set ID
/// @returns "true" if database result set ID is valid, otherwise "false"
bool DatabasesComponent::isDatabaseResultSetIDValid(int databaseResultSetID) const {
	return databaseResultSets.valid(databaseResultSetID);
}

/// Gets a database result set by ID
/// @param databaseResultSetID Database result set ID
/// @returns Database result set
IDatabaseResultSet& DatabasesComponent::getDatabaseResultSetByID(int databaseResultSetID) {
	return databaseResultSets.get(databaseResultSetID);
}

COMPONENT_ENTRY_POINT() {
	return &databaseComponent;
}