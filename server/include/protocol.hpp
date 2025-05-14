enum class DoorState : uint8_t {
    OPEN = 1,
    CLOSED = 2
};

std::string door_state_to_string(DoorState state) {
    return (state == DoorState::OPEN) ? "OPEN" : "CLOSED";
}