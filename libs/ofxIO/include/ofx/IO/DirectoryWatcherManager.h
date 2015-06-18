// =============================================================================
//
// Copyright (c) 2010-2013 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include <map>
#include "ofx/DirectoryWatcher.h"
#include "Poco/Exception.h"
#include "Poco/Path.h"
#include "ofTypes.h"
#include "ofEvents.h"
#include "ofx/IO/DirectoryUtils.h"
#include "ofx/IO/AbstractTypes.h"


namespace ofx {
namespace IO {


/// \brief A collection of directory watcher events.
/// \sa DirectoryWatcherManager::registerAllEvents()
/// \sa DirectoryWatcherManager::unregisterAllEvents()
/// \sa ofAddListener()
/// \sa ofRemoveListener()
class DirectoryWatcherEvents
{
public:
    /// \brief A typedef for a Poco::DirectoryWatcher::DirectoryEvent.
    typedef DirectoryWatcher::DirectoryEvent DirectoryEvent;

    /// \brief Create a DirectoryWatcherEvents.
    DirectoryWatcherEvents()
    {
    }

    /// \brief Destroy the DirectoryWatcherEvents.
    virtual ~DirectoryWatcherEvents()
    {
    }

    /// \brief Called when an item is added.
    ofEvent<const DirectoryEvent> onItemAdded;

    /// \brief Called when an item is removed.
    ofEvent<const DirectoryEvent> onItemRemoved;

    /// \brief Called when an item is modified.
    ofEvent<const DirectoryEvent> onItemModified;

    /// \brief Called when an item is moved from one place to another.
    /// \note This is not active on all systems.
    ofEvent<const DirectoryEvent> onItemMovedFrom;

    /// \brief Called when an item is move to another location.
    /// \note This is not active on all systems.
    ofEvent<const DirectoryEvent> onItemMovedTo;

    /// \brief Called when a directory error is encountered.
    ofEvent<const Poco::Exception> onScanError;

};


/// \brief A thread-safe collection of directory watchers.
/// \sa Poco::DirectoryWatcher
class DirectoryWatcherManager
{
public:
    /// \brief A typedef for a Poco::DirectoryWatcher::DirectoryEvent.
    typedef DirectoryWatcher::DirectoryEvent DirectoryEvent;

    /// \brief A directory type event.
    enum DirectoryEventType
	{
        /// \brief A new item has been created and added to the directory.
		ITEM_ADDED = DirectoryWatcher::DW_ITEM_ADDED,

        /// \brief An item has been removed from the directory.
		ITEM_REMOVED = DirectoryWatcher::DW_ITEM_REMOVED,

        /// An item has been modified.
		ITEM_MODIFIED = DirectoryWatcher::DW_ITEM_MODIFIED,

        /// \brief An item has been renamed or moved.
        ///        This event delivers the old name.
		ITEM_MOVED_FROM = DirectoryWatcher::DW_ITEM_MOVED_FROM,

        /// \brief An item has been renamed or moved.
        ///        This event delivers the new name.
		ITEM_MOVED_TO = DirectoryWatcher::DW_ITEM_MOVED_TO,
	};

    /// \brief A collection of directory event masks.
    enum DirectoryEventMask
	{
        /// \brief Enables all event types.
		FILTER_ENABLE_ALL = DirectoryWatcher::DW_FILTER_ENABLE_ALL,

        /// \brief Disables all event types.
		FILTER_DISABLE_ALL = DirectoryWatcher::DW_FILTER_DISABLE_ALL
	};

	enum
	{
        /// \brief Default scan interval for platforms that don't
        ///        provide a native notification mechanism.
		DEFAULT_SCAN_INTERVAL = DirectoryWatcher::DW_DEFAULT_SCAN_INTERVAL
	};

    /// \brief Create an empty DirectoryWatcherManager.
    DirectoryWatcherManager();

    /// \brief Destroy a DirectoryWatcherManager.
    virtual ~DirectoryWatcherManager();

    /// \brief Add a path to the watch list in order to receive related events.
    /// \param path The path to add and watch.
    /// \param listExistingItemsOnStart will fire ITEM_ADDED events for
    ///        matching items in the directory upon startup.
    /// \param sortAlphaNumeric sorts all values alphanumerically.
    /// \param pFilter is the path filter for this path.  The
    ///        DirectoryWatcherManager does not take  ownership
    ///        ownership of the pointer.
    /// \param eventMask defines the behavior of the
    ///        DirectoryWatcherManager for this Path.
    /// \param scanInterval specifies the interval in seconds between scans
    ///        for platforms that don't provide native notification
    ///        mechanisms.
    void addPath(const Poco::Path& path,
                 bool listExistingItemsOnStart = false,
                 bool sortAlphaNumeric         = false,
                 AbstractPathFilter* pFilter   = 0,
                 int  eventMask                = FILTER_ENABLE_ALL,
                 int  scanInterval             = DEFAULT_SCAN_INTERVAL);

    /// \brief Remove a path from the watch list.
    /// \param path to be removed.
    void removePath(const Poco::Path& path);

    /// \brief Query if a path is on the watch list.
    /// \param path to be checked.
    /// \returns true iff the path is on the watch list.
    bool isWatching(const Poco::Path& path) const;

    /// \brief An event object for Directory events.
    /// \sa ofAddListener()
    /// \sa ofRemoveListener()
    /// \sa registerAllEvents()
    /// \sa unregisterAllEvents()
    DirectoryWatcherEvents events;

    /// \brief Register a class to receive notifications for all events.
    /// \param listener a pointer to the listener class.
    /// \param order the event order.
    template<class ListenerClass>
    void registerAllEvents(ListenerClass* listener, int order = OF_EVENT_ORDER_AFTER_APP)
    {
        ofAddListener(events.onItemAdded, listener, &ListenerClass::onDirectoryWatcherItemAdded, order);
        ofAddListener(events.onItemRemoved, listener, &ListenerClass::onDirectoryWatcherItemRemoved, order);
        ofAddListener(events.onItemModified, listener, &ListenerClass::onDirectoryWatcherItemModified, order);
        ofAddListener(events.onItemMovedFrom, listener, &ListenerClass::onDirectoryWatcherItemMovedFrom, order);
        ofAddListener(events.onItemMovedTo, listener, &ListenerClass::onDirectoryWatcherItemMovedTo, order);
        ofAddListener(events.onScanError,listener, &ListenerClass::onDirectoryWatcherError, order);
    }

    /// \brief Unregister a class to receive notifications for all events.
    /// \param listener a pointer to the listener class.
    template<class ListenerClass>
    void unregisterAllEvents(ListenerClass* listener)
    {
        ofRemoveListener(events.onItemAdded, listener, &ListenerClass::onDirectoryWatcherItemAdded);
        ofRemoveListener(events.onItemRemoved, listener, &ListenerClass::onDirectoryWatcherItemRemoved);
        ofRemoveListener(events.onItemModified, listener, &ListenerClass::onDirectoryWatcherItemModified);
        ofRemoveListener(events.onItemMovedFrom, listener, &ListenerClass::onDirectoryWatcherItemMovedFrom);
        ofRemoveListener(events.onItemMovedTo, listener, &ListenerClass::onDirectoryWatcherItemMovedTo);
        ofRemoveListener(events.onScanError, listener, &ListenerClass::onDirectoryWatcherError);
    }

protected:
    /// \brief Get a matching filter for the given path.
    /// \param path The path to match.
    /// \returns A pointer to the matching AbstractPathFilter.
    AbstractPathFilter* getFilterForPath(const Poco::Path& path);

    /// \brief Called when an item is added.
    /// \param evt A Poco::DirectoryWatcher::DirectoryEvent.
    void onItemAdded(const DirectoryWatcher::DirectoryEvent& evt)
    {
        ofNotifyEvent(events.onItemAdded, evt, this);
    }

    /// \brief Called when an item is removed.
    /// \param evt A Poco::DirectoryWatcher::DirectoryEvent.
    void onItemRemoved(const DirectoryWatcher::DirectoryEvent& evt)
    {
        ofNotifyEvent(events.onItemRemoved, evt, this);
    }

    /// \brief Called when an item is modified.
    /// \param evt A Poco::DirectoryWatcher::DirectoryEvent.
    void onItemModified(const DirectoryWatcher::DirectoryEvent& evt)
    {
        ofNotifyEvent(events.onItemModified, evt, this);
    }

    /// \brief Called when an item is moved from one location to another.
    /// \param evt A Poco::DirectoryWatcher::DirectoryEvent.
    /// \note Not implemented on all platforms.
    void onItemMovedFrom(const DirectoryWatcher::DirectoryEvent& evt)
    {
        ofNotifyEvent(events.onItemMovedFrom, evt, this);
    }

    /// \brief Called when an item is moved from one location to another.
    /// \param evt A Poco::DirectoryWatcher::DirectoryEvent.
    /// \note Not implemented on all platforms.
    void onItemMovedTo(const DirectoryWatcher::DirectoryEvent& evt)
    {
        ofNotifyEvent(events.onItemMovedTo, evt, this);
    }

    /// \brief Called when a directory watcher error is detected.
    /// \param exc A Poco::Exception.
    void onScanError(const Poco::Exception& exc)
    {
        ofNotifyEvent(events.onScanError, exc, this);
    }

private:
    typedef std::shared_ptr<DirectoryWatcher> DirectoryWatcherPtr;
    typedef std::map<Poco::File, DirectoryWatcherPtr> WatchList;
    typedef WatchList::iterator WatchListIter;
    typedef std::map<Poco::File, AbstractPathFilter*> FilterList;
    typedef FilterList::iterator FilterListIter;

    /// \brief A map of the files and their associated watchers.
    WatchList  watchList;

    /// \brief A collection of filters applied to the watching activities.
    FilterList filterList;

    /// \brief A mutex for mutithreaded processing.
    mutable std::mutex _mutex;

};


} } // namespace ofx::IO
