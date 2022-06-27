#include "../../Utilities/Math.h"
#include <iostream>

int main() {
    const static int FAILURE = 1;
    const static int SUCCESS = 0;
    
    Utilities::DataTypes::Mat4 A;
    Utilities::DataTypes::Mat4 B;
    Utilities::DataTypes::Mat4 correct_result;
    Utilities::DataTypes::Mat4 answer;
    const float correct_results_determinant = -5154875.0; // As calculated by a TI-84 calculator

    A.data[0][0] = 8;
    A.data[0][1] = 2;
    A.data[0][2] = 3;
    A.data[0][3] = 7;

    A.data[1][0] = 4;
    A.data[1][1] = 9;
    A.data[1][2] = 1;
    A.data[1][3] = 0;

    A.data[2][0] = 3;
    A.data[2][1] = 2;
    A.data[2][2] = 4;
    A.data[2][3] = 6;

    A.data[3][0] = 3;
    A.data[3][1] = 0;
    A.data[3][2] = 7;
    A.data[3][3] = 4;

    B.data[0][0] = 5;
    B.data[0][1] = 7;
    B.data[0][2] = 9;
    B.data[0][3] = 0;

    B.data[1][0] = 4;
    B.data[1][1] = 6;
    B.data[1][2] = 8;
    B.data[1][3] = 7;

    B.data[2][0] = 9;
    B.data[2][1] = 2;
    B.data[2][2] = 3;
    B.data[2][3] = 4;

    B.data[3][0] = 8;
    B.data[3][1] = 9;
    B.data[3][2] = 1;
    B.data[3][3] = 3;

    correct_result.data[0][0] = 131;
    correct_result.data[0][1] = 137;
    correct_result.data[0][2] = 104;
    correct_result.data[0][3] =  47;

    correct_result.data[1][0] =  65;
    correct_result.data[1][1] =  84;
    correct_result.data[1][2] = 111;
    correct_result.data[1][3] =  67;

    correct_result.data[2][0] = 107;
    correct_result.data[2][1] =  95;
    correct_result.data[2][2] =  61;
    correct_result.data[2][3] =  48;

    correct_result.data[3][0] = 110;
    correct_result.data[3][1] =  71;
    correct_result.data[3][2] =  52;
    correct_result.data[3][3] =  40;

    Utilities::Math::multiply( answer, A, B );

    float result = 0;
    float sum;

    for( unsigned int x = 0; x < 4; x++ )
    {
        for( unsigned int y = 0; y < 4; y++ )
        {
            sum = answer.data[x][y] - correct_result.data[x][y];
            result += (sum >= 0.0) * sum + (sum < 0.0) * -sum;
        }
    }

    if( result >= 1.0 || result <= -1.0f ) {
        std::cout << "The multiply routine had failed. Review Math.cpp!" << std::endl;
        return FAILURE;
    }

    // Normalize so the correct answer is between 1.0 to -1.0.
    result = Utilities::Math::determinant( correct_result ) - correct_results_determinant;

    if( result >= 1.0 || result <= -1.0f ) {
        std::cout << "The determinant operation failed. Review Math.cpp!" << std::endl;
        return FAILURE;
}

    // A will store the cofactor of correct_result.
    A.data[0][0] =   -7413;
    A.data[0][1] =   19817;
    A.data[0][2] =  111149;
    A.data[0][3] = -157862;

    A.data[1][0] =  -30578;
    A.data[1][1] =   42802;
    A.data[1][2] = -206306;
    A.data[1][3] =  211803;

    A.data[2][0] =  -81671;
    A.data[2][1] =  -54261;
    A.data[2][2] =  253108;
    A.data[2][3] = -116879;

    A.data[3][0] =  180834;
    A.data[3][1] =  -59931;
    A.data[3][2] = -268507;
    A.data[3][3] =   81241;

    // Store the inversion answer in the B matrix
    Utilities::Math::invert( B, correct_result );

    result = 0;

    for( unsigned int x = 0; x < 4; x++ )
    {
        for( unsigned int y = 0; y < 4; y++ )
        {
            // If the inversion of the matrix gets multiplied by its determinant then it is a cofactor matrix.
            sum = A.data[x][y] - (B.data[x][y] * correct_results_determinant);
            result += (sum >= 0.0) * sum + (sum < 0.0) * -sum;
        }
    }

    if( result >= 1.0 || result <= -1.0f ) {
        std::cout << "The inverse operation failed. Review Math.cpp!" << std::endl;
        return FAILURE;
    }

    return SUCCESS;
}

