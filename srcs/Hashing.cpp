/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hashing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmicheli <lmicheli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 18:30:29 by lmicheli          #+#    #+#             */
/*   Updated: 2024/07/19 12:17:17 by lmicheli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <main.hpp>

// Function to convert a byte array to a hex string
std::string byte_array_to_hex_string(const unsigned char* byte_array, size_t length)
{
    std::ostringstream oss;
    for (size_t i = 0; i < length; ++i) {
        oss << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(byte_array[i]);
    }
    return oss.str();
}

// Get salt ( idk what it is )
std::string generate_salt(size_t length)
{
    std::string salt;
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (size_t i = 0; i < length; ++i) {
        salt += charset[rand() % (sizeof(charset) - 1)];
    }
    return salt;
}

// Hash password
std::string hash_password(const std::string& password, const std::string& salt)
{
    std::string salted_password = password + salt;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(salted_password.c_str()), salted_password.length(), hash);
    return byte_array_to_hex_string(hash, SHA256_DIGEST_LENGTH);
}

// Check if password is correct
bool verify_password(const std::string& entered_password, const std::string& stored_hash, const std::string& salt) {
    std::string hashed_entered_password = hash_password(entered_password, salt);
    return hashed_entered_password == stored_hash;
}
// Example usage
// void example_usage() {
//     std::string password = "my_secure_password";
//     std::string salt = generate_salt(16); // Generate a 16-character salt
//     std::string hashed_password = hash_password(password, salt);
//     store_hashed_password("username", hashed_password, salt);
// }

// Function to verify password