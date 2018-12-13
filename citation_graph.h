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
    CitationGraph(typename Publication::id_type const &stem_id) noexcept : root(std::make_shared(Publication(stem_id))) {}

    CitationGraph(CitationGraph<Publication> &&other) : root(std::move(other.root)) {}

    CitationGraph(CitationGraph<Publication> &other) = delete;

    ~CitationGraph() { root.reset(); }

    CitationGraph<Publication> &operator=(CitationGraph<Publication> &other) = delete;

    CitationGraph<Publication> &operator=(CitationGraph<Publication> &&other) noexcept;

    Publication &operator[](typename Publication::id_type const &id) const;

    typename Publication::id_type get_root_id() const noexcept(noexcept(Publication::get_id()));

    std::vector<typename Publication::id_type> get_children(typename Publication::id_type const &id) const;

    std::vector<typename Publication::id_type> get_parents(typename Publication::id_type const &id) const;

    bool exists(typename Publication::id_type const &id) const;

    void create(typename Publication::id_type const &id, typename Publication::id_type const &parent_id);

    void create(typename Publication::id_type const &id, std::vector<typename Publication::id_type> const &parent_ids);

    void add_citation(typename Publication::id_type const &child_id, typename Publication::id_type const &parent_id);

    void remove(typename Publication::id_type const &id);

private:
    class Node {
    private:
        Publication publication;
        // Publickacje które ją cytują.
        std::vector<std::shared_ptr<Node> > citations;
    public:
        Node(Publication publication) : publication(publication) {}

        void add_child(std::shared_ptr<Node> another) { citations.emplace_back(another); }

        typename Publication::id_type getStem_id() const { return publication.get_id(); }

        Publication getPublication() const { return publication; }

        const std::vector<std::shared_ptr<Node> > &getCitations() const { return citations; }

        // Znalezienie publikacji w grafie. Jeśli nie istnieje zwraca nullptr.
        std::shared_ptr<Node> find(typename Publication::id_type id) noexcept;
    };

    std::shared_ptr<Node> root;
};

template<class Publication>
typename Publication::id_type CitationGraph<Publication>::get_root_id() const noexcept(noexcept(Publication::get_id())) {
    //TODO jakiś try/catch?
    return root->getStem_id();
}

template<class Publication>
std::vector<typename Publication::id_type> CitationGraph<Publication>::get_parents(const typename Publication::id_type &id) const {
    std::shared_ptr<Node> node = root->find(id);

    if (node.get() == nullptr) {
        throw PublicationNotFound();
    } else {
        std::vector<typename Publication::id_type> out;

        for (auto it : node->getCitations()) {
            out.push_back(it->get_id());
        }

        return out;
    }
}

template<class Publication>
bool CitationGraph<Publication>::exists(const typename Publication::id_type &id) const {
    if (root->find(id).get() == nullptr) {
        return false;
    } else {
        return true;
    }
}

template<class Publication>
void CitationGraph<Publication>::add_citation(const typename Publication::id_type &child_id,
                                              const typename Publication::id_type &parent_id) {
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
Publication &CitationGraph<Publication>::operator[](const typename Publication::id_type &id) const {
    std::shared_ptr<Node> publication = root->find(id);

    if (publication.get() != nullptr) {
        return publication->getPublication();
    } else {
        throw PublicationNotFound();
    }

}

template<class Publication>
void CitationGraph<Publication>::create(const typename Publication::id_type &id, const typename Publication::id_type &parent_id) {
    if(root -> find(id).get() != nullptr) { throw PublicationAlreadyCreated(); }

    std::shared_ptr<Node> parent = root -> find(parent_id);

    if(parent.get() == nullptr) { throw PublicationNotFound(); }
    else{
        parent -> add_child(std::make_shared(Publication(id)));
    }
}

template<class Publication>
void CitationGraph<Publication>::create(const typename Publication::id_type &id, const std::vector<typename Publication::id_type> &parent_ids) {
    if(root -> find(id).get() != nullptr) { throw PublicationAlreadyCreated(); }

    std::vector<std::shared_ptr<Node>> parents;

    for(auto & it : parent_ids) {
        std::shared_ptr<Node> parent = root -> find(it);

        if(parent.get() == nullptr) { throw PublicationNotFound(); }
        else{
            parents.emplace_back(parent);
        }
    }

    // Jak nie rzuciło do dej pory wyjątku to można bezpiecznie dodać.
    for(auto & it : parents) {
        it -> add_child(std::make_shared(Publication(id)));
    }
}

#endif //JNP1_5_CITATION_GRAPH_H
