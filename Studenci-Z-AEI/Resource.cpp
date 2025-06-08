#include "Resource.h"
#include <string>

std::string resourceName(ResourceType type) {
    switch (type) {
        case ResourceType::Kawa: return "Kawa";
        case ResourceType::Piwo: return "Piwo";
        case ResourceType::Notatki: return "Notatki";
        case ResourceType::Pizza: return "Pizza";
        case ResourceType::Kabel: return "Kabel";
        default: return "Brak";
    }
}