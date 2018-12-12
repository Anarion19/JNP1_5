//
// Created by kacper on 10.12.18.
//

#ifndef JNP1_5_CITATION_GRAPH_H
#define JNP1_5_CITATION_GRAPH_H

#include <vector>
#include <exception>
#include <memory>

class PublicationAlreadyCreated : std::exception {
    const char *what() const noexcept {
        return "PublicationAlreadyCreated";
    }
};

class PublicationNotFound : std::exception {
    const char *what() const noexcept {
        return "PublicationNotFound";
    }
};

class TriedToRemoveRoot : std::exception {
    const char *what() const noexcept {
        return "TriedToRemoveRoot";
    }
};

template<class Publication>
class CitationGraph {
public:
    CitationGraph(Publication::id_type const &stem_id) noexcept : root(std::make_shared(Publication(stem_id))) {}

    CitationGraph(CitationGraph<Publication> &&other) : root(std::move(other.root)) {}

    CitationGraph(CitationGraph<Publication> &other) = delete;

    ~CitationGraph() { root.reset(); }

    CitationGraph<Publication> &operator=(CitationGraph<Publication> &other) = delete;

    CitationGraph<Publication> &operator=(CitationGraph<Publication> &&other) noexcept;

    Publication &operator[](Publication::id_type const &id) const;

    Publication::id_type get_root_id() const noexcept(noexcept(Publication::get_id()));

    std::vector<Publication::id_type> get_children(Publication::id_type const &id) const;

    std::vector<Publication::id_type> get_parents(Publication::id_type const &id) const;

    bool exists(Publication::id_type const &id) const;

    void create(Publication::id_type const &id, Publication::id_type const &parent_id);

    void create(Publication::id_type const &id, std::vector<Publication::id_type> const &parent_ids);

    void add_citation(Publication::id_type const &child_id, Publication::id_type const &parent_id);

    void remove(Publication::id_type const &id);

private:
    class Node {
    private:
        Publication publication;
        // Publickacje które ją cytują.
        std::vector<std::shared_ptr<Node> > citations;
    public:
        Node(Publication publication) : publication(publication) {}

        void add_child(std::shared_ptr<Node> another) { citations.emplace_back(another); }

        Publication::id_type getStem_id() const { return publication.get_id(); }

        Publication getPublication() const { return publication; }

        const std::vector<std::shared_ptr<Node> > &getCitations() const { return citations; }

        // Znalezienie publikacji w grafie. Jeśli nie istnieje zwraca nullptr.
        std::shared_ptr<Node> find(Publication::id_type id) noexcept;
    };

    std::shared_ptr<Node> root;
};

template<class Publication>
Publication::id_type CitationGraph<Publication>::get_root_id() const noexcept(noexcept(Publication::get_id())) {
    //TODO jakiś try/catch?
    return root->getStem_id();
}

template<class Publication>
std::vector<Publication::id_type> CitationGraph<Publication>::get_parents(const Publication::id_type &id) const {
    std::shared_ptr<Node> node = root->find(id);

    if (node.get() == nullptr) {
        throw PublicationNotFound();
    } else {
        std::vector<Publication::id_type> out;

        for (auto it : node->getCitations()) {
            out.push_back(it->get_id());
        }

        return out;
    }
}

template<class Publication>
bool CitationGraph<Publication>::exists(const Publication::id_type &id) const {
    if (root->find(id).get() == nullptr) {
        return false;
    } else {
        return true;
    }
}

template<class Publication>
void CitationGraph<Publication>::add_citation(const Publication::id_type &child_id,
                                              const Publication::id_type &parent_id) {
    std::shared_ptr<Node> parent = root->find(parent_id);

    if (parent.get() != nullptr) {
        std::shared_ptr<Node> child = root->find(child_id);

        if (child.get() != nullptr) {
            parent->add_child(child);
        } else {
            throw PublicationNotFound();
        }
    } else {
        throw PublicationNotFound();
    }
}

template<class Publication>
Publication &CitationGraph<Publication>::operator[](const Publication::id_type &id) const {
    std::shared_ptr<Node> publication = root->find(id);

    if (publication.get() != nullptr) {
        return publication->getPublication();
    } else {
        throw PublicationNotFound();
    }

}

#endif //JNP1_5_CITATION_GRAPH_H
