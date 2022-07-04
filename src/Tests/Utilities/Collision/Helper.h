#include <ostream>

const float TOLERENCE = 0.0078125f;

bool isNotMatch( float op1, float op2, float tolerence = TOLERENCE ) {
    return ( op1 > op2 + tolerence) | (op1 < op2 - tolerence );
}

bool isNotMatch( const glm::vec3& op1, const glm::vec3& op2, float tolerence = TOLERENCE ) {
    bool is_not_valid = false;
    
    is_not_valid |= isNotMatch( op1.x, op2.x, tolerence );
    is_not_valid |= isNotMatch( op1.y, op2.y, tolerence );
    is_not_valid |= isNotMatch( op1.z, op2.z, tolerence );
    
    return is_not_valid;
}

void displayVec3( std::string name, const glm::vec3& display, std::ostream &output  ) {
    output << name << ".x = " << display.x << std::endl;
    output << name << ".y = " << display.y << std::endl;
    output << name << ".z = " << display.z << std::endl;
}
