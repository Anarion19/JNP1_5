#ifndef JNP1_5_CITATION_GRAPH_H
#define JNP1_5_CITATION_GRAPH_H

#include <vector>
#include <exception>
#include <memory>
#include <map>
#include <iostream>
#include <set>

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
    CitationGraph(typename Publication::id_type const &stem_id) noexcept;

    CitationGraph(CitationGraph<Publication> &&other) noexcept : root(std::move(other.root)),
                                                                 graph(std::move(other.graph)) {}

    CitationGraph(CitationGraph<Publication> &other) = delete;

    ~CitationGraph() { root.reset(); }

    CitationGraph<Publication> &operator=(CitationGraph<Publication> &other) = delete;

    CitationGraph<Publication> &operator=(CitationGraph<Publication> &&other) noexcept;

    Publication &operator[](typename Publication::id_type const &id) const;

    //TODO nie działa
    typename Publication::id_type get_root_id() const noexcept(noexcept(std::declval<Publication>().get_id()));

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
        std::set<std::shared_ptr<Node>> parents;
        std::set<std::shared_ptr<Node>> children;
        int point;
    public:
        Node(Publication publication) : publication(publication) {
            static int i = 0;
            i++;
            point = i;
        }

        void add_child(std::shared_ptr<Node> another) { children.insert(children.begin(), another); }

        void add_parent(std::shared_ptr<Node> another) { parents.insert(parents.begin(), another); }

        typename Publication::id_type getStem_id() const { return publication.get_id(); }

        const Publication &getPublication() const { return publication; }

        int getPoint() const { return point; }

        const std::set<std::shared_ptr<Node> > &getParensts() const { return parents; }

        const std::set<std::shared_ptr<Node> > &getChildren() const { return children; }

        bool isOrphan() const { return parents.empty(); }

        void clear() {
            parents.clear();
            children.clear();
        }

        void visit() {
            if (point > 0) {
                point *= -1;

                for (auto &it : children) {
                    it->second.visit();
                }
            }
        }

        void reset() { point *= -1; }

        bool operator<(const Node &b) { return point < b.point; }
    };

    std::shared_ptr<Node> root;
    std::map<typename Publication::id_type, std::shared_ptr<Node>> graph;
};

template<class Publication>
typename Publication::id_type
CitationGraph<Publication>::get_root_id() const noexcept(noexcept(std::declval<Publication>().get_id())) {
    return root->getStem_id();
}

template<class Publication>
std::vector<typename Publication::id_type>
CitationGraph<Publication>::get_parents(const typename Publication::id_type &id) const {
    auto node = graph.find(id);

    if (node == graph.end()) {
        throw PublicationNotFound();
    } else {
        std::vector<typename Publication::id_type> out;

        for (auto it : node->second->getParensts()) {
            out.push_back(it.get()->getStem_id());
        }

        return out;
    }
}

template<class Publication>
bool CitationGraph<Publication>::exists(const typename Publication::id_type &id) const {
    return !(graph.find(id) == graph.end());
}

template<class Publication>
void CitationGraph<Publication>::add_citation(const typename Publication::id_type &child_id,
                                              const typename Publication::id_type &parent_id) {
    auto parent = graph.find(parent_id);

    if (parent != graph.end()) {
        auto child = graph.find(child_id);

        if (child != graph.end()) {
            parent->second->add_child(child->second);
            child->second->add_parent(parent->second);
        } else {
            throw PublicationNotFound();
        }
    } else {
        throw PublicationNotFound();
    }
}

template<class Publication>
Publication &CitationGraph<Publication>::operator[](const typename Publication::id_type &id) const {
    auto publication = graph.find(id);

    if (publication != graph.end()) {
        return const_cast <Publication & >(publication->second->getPublication());
    } else {
        throw PublicationNotFound();
    }

}

template<class Publication>
void CitationGraph<Publication>::create(const typename Publication::id_type &id,
                                        const typename Publication::id_type &parent_id) {
    if (graph.find(id) != graph.end()) { throw PublicationAlreadyCreated(); }
    std::shared_ptr<Node> child = std::make_shared<Node>(id);
    auto parent = graph.find(parent_id);

    if (parent == graph.end()) { throw PublicationNotFound(); }
    else {
        child->add_parent(parent->second);
        graph[id] = child;
        parent->second->add_child(std::make_shared<Node>(Publication(id)));
    }
}

template<class Publication>
void CitationGraph<Publication>::create(const typename Publication::id_type &id,
                                        const std::vector<typename Publication::id_type> &parent_ids) {
    if (graph.find(id) != graph.end()) { throw PublicationAlreadyCreated(); }
    std::shared_ptr<Node> child = std::make_shared<Node>(id);
    std::vector<std::shared_ptr<Node>> parents;

    for (auto &it : parent_ids) {
        auto parent = graph.find(it);
        // Sprawdzenie czy wszyscy rodzice istnieją.
        if (parent == graph.end()) { throw PublicationNotFound(); }
        else {
            parents.emplace_back(parent->second);
        }
    }
    graph[id] = child;
    // Jak nie rzuciło do dej pory wyjątku to można bezpiecznie dodać.
    for (auto &it : parents) {
        child->add_parent(it);
        it->add_child(child);
    }
}

template<class Publication>
CitationGraph<Publication>::CitationGraph(const typename Publication::id_type &stem_id) noexcept {
    root = std::make_shared<Node>(Publication(stem_id));
    graph[stem_id] = root;
}

template<class Publication>
std::vector<typename Publication::id_type>
CitationGraph<Publication>::get_children(const typename Publication::id_type &id) const {
    auto node = graph.find(id);

    if (node == graph.end()) {
        throw PublicationNotFound();
    } else {
        std::vector<typename Publication::id_type> out;

        for (auto it : node->second->getChildren()) {
            out.push_back(it.get()->getStem_id());
        }

        return out;
    }
}

//TODO u mnie rzuca się że nie działa a powinno bo reszta jest tak napisana i kurwa działa
template<class Publication>
void CitationGraph<Publication>::remove(const typename Publication::id_type &id) {
    auto item = graph.find(id);

    if (item == graph.end()) {
        throw PublicationNotFound();
    } else {
        if (item->second->getStem_id() == root->getStem_id()) { throw TriedToRemoveRoot(); }

        item->second->clear();
        graph.erase(item);

        root->visit();

        for (auto &it : graph) {
            if (it->second->getPoint() > 0) {
                it->second->clear();
                it = graph.erase(it);
                it--;
            } else {
                it->second->reset();
            }
        }

    }
}

#endif
