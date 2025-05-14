enum class DoorState : uint8_t {
    OPEN : 1,
    CLOSED = 2
};

std::string to_string(DoorState state) {
    return (state == DoorState::OPEN) ? "OPEN" : "CLOSED";
}