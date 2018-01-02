#ifndef UTILS_H
#define UTILS_H

static const IPAddress NO_IP(0, 0, 0, 0);

String getCoreID() {
    String coreIdentifier = "";
    char id[12];
    memcpy(id, (char *) ID1, 12);
    char hex_digit;
    for (int i = 0; i < 12; ++i) {
        hex_digit = 48 + (id[i] >> 4);
        if (57 < hex_digit)
            hex_digit += 39;
        coreIdentifier = coreIdentifier + hex_digit;
        hex_digit = 48 + (id[i] & 0xf);
        if (57 < hex_digit)
            hex_digit += 39;
        coreIdentifier = coreIdentifier + hex_digit;
    }
    return coreIdentifier;
}

bool compareIgnoringCase(String s1, String s2) {
    return s1.toUpperCase() == s2.toUpperCase();
}

bool ipEquals(IPAddress ip1, IPAddress ip2) {
    return ip1.raw().ipv4 == ip2.raw().ipv4;
}

bool validIp(IPAddress ip) {
    return !ipEquals(ip, NO_IP);
}

IPAddress toIP(String input) {
    int parts[4] = {0, 0, 0, 0};
    int part = 0;
    for (unsigned i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '.') {
            part++;
            continue;
        }
        parts[part] *= 10;
        parts[part] += c - '0';
    }
    IPAddress ip(parts[0], parts[1], parts[2], parts[3]);
    return ip;
}

String fromMac(byte mac[6]) {
    char hex_digit;
    String result = "";
    for (int i = 0; i < 6; ++i) {
        if (i != 0) {
            result += ":";
        }
        hex_digit = 48 + (mac[i] >> 4);
        if (57 < hex_digit) {
            hex_digit += 39;
        }
        result = result + hex_digit;
        hex_digit = 48 + (mac[i] & 0xf);
        if (57 < hex_digit) {
            hex_digit += 39;
        }
        result = result + hex_digit;
    }
    return result.toUpperCase();
}

char convertCharToHex(char input) {
    if (input >= '0' && input <= '9')
        return input - '0';
    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;
}

bool toMac(String value, byte input[6]) {
    for (int i = 0; i < 6; ++i) {
        byte b1 = convertCharToHex(value.charAt(3 * i));
        byte b2 = convertCharToHex(value.charAt(3 * i + 1));

        if (b1 > 16) {
            return false;
        }

        if (b2 > 16) {
            return false;
        }

        input[i] = b1 << 4 | b2;
    }
    return true;
}

#endif