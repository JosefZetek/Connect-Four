#include "../data_structures/vector.h"
#include "../data_structures/game_structure.h"
#include "../data_structures/unconfirmed_request.h"

#include "message_formatter.h"
#include "networking.h"

#include "../constants.h"

void handle_message(int sockfd, struct vector * games, struct game_structure ** lobby);
